#include "compress_huffman_adaptive.h"
#include "bytebuffer.h"
#include "tree.h"

#include <stdexcept>
#include <iostream>
#include <queue>
#include <vector>
#include <unordered_map>

class TreeWeightComparator
{
public:
    bool operator()(const Tree& val1, const Tree & val2) const
    {
        return val1.CalcWeight() > val2.CalcWeight();
    }
};


bool CompressHuffmanAdaptive::CompressImpl(const ByteBuffer& input1, ByteBuffer& output)
{
    typedef std::unordered_map<char, uint32> CharacterMap;
    typedef std::unordered_map<char, std::deque<bool>> CodeMap;
    typedef std::priority_queue<Tree, std::vector<Tree>, TreeWeightComparator> TreeMinHeap; 

    CharacterMap charMap;
    ByteBuffer& input = const_cast<ByteBuffer&>(input1);

    uint readCharacters = 0;
    char c;

    CharacterMap::iterator itr;

    while (input.CanRead())
    {
        c = input.ReadInt8();

        itr = charMap.find(c);

        if (itr != charMap.end())
            itr->second++;
        else
            charMap.insert(itr, std::make_pair(c, 1));

        readCharacters++;
    }

    input.SetReadPos(input.GetReadPos() - readCharacters);

    TreeMinHeap treeHeap;

    for (CharacterMap::const_reference elem : charMap)
        treeHeap.push(Tree(new Tree::Node(elem.first, elem.second)));

    while (treeHeap.size() > 1)
    {
        Tree z (new Tree::Node());
        z.Root->LeftChild = treeHeap.top().Root;
        treeHeap.pop();
        z.Root->RightChild = treeHeap.top().Root;
        treeHeap.pop();
        treeHeap.push(z);
    }

    Tree bt = treeHeap.top();

    CodeMap codes;

    for (CharacterMap::const_reference elem : charMap)
    {
        codes[elem.first] = bt.Find(elem.first);
    }

    output.WriteDynInt(charMap.size());
    for (const auto& elem: charMap)
    {
        output.WriteInt8(elem.first);
        output.WriteDynInt(elem.second);
    }

    size_t numberPos = output.GetWritePos();
    output.WriteUInt64(0);
    size_t numBitsWritten = 0;

    while (input.CanRead())
    {
        c = input.ReadInt8();

        for (const bool& b : codes[c]) 
        {
            output.WriteBit(b);
            numBitsWritten++;
        }
    }


    output.FlushBits();

    size_t lastPost = output.GetWritePos();
    output.SetWritePos(numberPos);
    output.WriteUInt64(numBitsWritten);
    output.SetWritePos(lastPost);

    bt.DeleteAll();

    return true;
}

bool CompressHuffmanAdaptive::DecompressImpl(const ByteBuffer& input1, ByteBuffer& output)
{
    typedef std::priority_queue<Tree, std::vector<Tree>, TreeWeightComparator> TreeMinHeap; 

    TreeMinHeap treeHeap;

    ByteBuffer& input = const_cast<ByteBuffer&>(input1);

    size_t numOfChars = input.ReadDynInt();

    for (int i = 0; i < numOfChars; ++i)
    {
        char c = input.ReadInt8();
        uint32 num = input.ReadDynInt();

        treeHeap.push(Tree(new Tree::Node(c, num)));
    }

    size_t numOfBits = input.ReadUInt64();

    while (treeHeap.size() > 1)
    {
        Tree z (new Tree::Node());
        z.Root->LeftChild = treeHeap.top().Root;
        treeHeap.pop();
        z.Root->RightChild = treeHeap.top().Root;
        treeHeap.pop();
        treeHeap.push(z);
    }

    Tree bt = treeHeap.top();

    Tree::NodePtr node = bt.Root;

    for (int i = 0; i < numOfBits; ++i)
    {
        bool b = input.ReadBit();
        if (node->IsLeaf)
        {
            output.WriteInt8(node->Value);
            node = b ? bt.Root->RightChild : bt.Root->LeftChild;
        }
        else
        {
            node = b ? node->RightChild : node->LeftChild;
        }
    }

    output.WriteInt8(node->Value);

    bt.DeleteAll();

    return true;
}
