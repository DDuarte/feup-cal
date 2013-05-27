#include "tree.h"
#include "utils.h"

#include <stack>
#include <utility> // std::Pair
#include <queue>

Tree::Node::Node(char val, uint freq) : Value(val), Frequency(freq), IsLeaf(true), Parent(nullptr)
{ }

Tree::Node::Node() : IsLeaf(false), LeftChild(nullptr), RightChild(nullptr), Parent(nullptr)
{ }

Tree::Tree() : Root(nullptr)
{ }

Tree::Tree(Node* n) : Root(n)
{ }

uint32 Tree::CalcWeight() const
{
    std::stack<NodePtr> stk;
    uint32 result = 0;
    stk.push(Root);

    while(!stk.empty())
    {
        NodePtr node = stk.top();
        stk.pop();

        if (!node) continue;

        if (node->IsLeaf)
            result += node->Frequency;
        else
        {
            stk.push(node->LeftChild);
            stk.push(node->RightChild);
        }
    }

    return result;
}

std::deque<bool> Tree::Find(char val) const
{
    std::stack<NodePtr> np;

    np.push(Root);

    while (!np.empty())
    {
        NodePtr node = np.top(); np.pop();

        if (!node->IsLeaf)
        {
            node->RightChild->Parent = node;
            node->LeftChild->Parent = node;

            np.push(node->RightChild);
            np.push(node->LeftChild);
        }
    }

    std::stack<NodePtr> stk;
    std::deque<bool> result;
    stk.push(Root);

    NodePtr node = Root;

    while(!stk.empty())
    {
        node = stk.top(); stk.pop();


        if (!node) continue;

        if (node->IsLeaf && node->Value == val)
            break;
        else if (!node->IsLeaf)
        {
            stk.push(node->LeftChild);
            stk.push(node->RightChild);
        }
    }

    while (node != Root)
    {
        NodePtr parent = node->Parent;
        result.push_front(node == parent->RightChild);
        node = parent;
    }

    return result;
}

void Tree::DeleteAll()
{
    typedef std::pair<bool, NodePtr> AuxPair;
    std::stack<AuxPair> stk;

    stk.push(std::make_pair(false, Root));

    while(!stk.empty())
    {
        AuxPair aux = stk.top();
        NodePtr node = aux.second;

        stk.pop();

        if (!node) continue;

        if (node->IsLeaf || aux.first)
        {
            delete node;
        }
        else
        {
            stk.push(std::make_pair(true, node));
            stk.push(std::make_pair(false, node->LeftChild));
            stk.push(std::make_pair(false, node->RightChild));
        }
    }

    Root = nullptr;
}

void Tree::Save(ByteBuffer& output)
{
    std::queue<NodePtr> q;

    size_t pos = output.GetWritePos();
    size_t numberOfNodes = 0;

    output.WriteUInt64(0);

    q.push(Root);

    while (!q.empty())
    {
        NodePtr node = q.front(); q.pop();

        output.WriteBit(node->IsLeaf);
        numberOfNodes++;

        if (!node->IsLeaf)
        {
            q.push(node->LeftChild);
            q.push(node->RightChild);
        }
    }

    output.FlushBits();

    size_t pos2 = output.GetWritePos();
    output.SetWritePos(pos);
    output.WriteUInt64(numberOfNodes);
    output.SetWritePos(pos2);

    q.push(Root);

    while (!q.empty())
    {
        NodePtr node = q.front(); q.pop();

        if (node->IsLeaf)
        {
            output.WriteInt8(node->Value);
        }
        else
        {
            q.push(node->LeftChild);
            q.push(node->RightChild);
        }
    }
}

Tree Tree::Load(ByteBuffer& input)
{
    Tree t;
    size_t numberOfNodes = input.ReadUInt64();

    if (numberOfNodes == 0)
        return Tree();

    {
        bool b = input.ReadBit();
        if (b)
            t.Root = new Node(0, 0);
        else
            t.Root = new Node();
    }

    std::queue<NodePtr> q;
    NodePtr node = t.Root;

    for (size_t i = 1; i < numberOfNodes; ++i)
    {
        bool left = input.ReadBit();
        bool right = input.ReadBit();

        node->LeftChild = left ? new Node(0, 0) : new Node();
        if (!left) q.push(node->LeftChild);

        node->RightChild = right ? new Node(0, 0) : new Node();
        if (!right) q.push(node->RightChild);


        if (!q.empty())
        {
            node = q.front();
            q.pop();
        }
        else break;

    }

    while (!q.empty()) q.pop();

    q.push(t.Root);

    while (!q.empty())
    {
        NodePtr n = q.front();
        q.pop();

        if (n->IsLeaf)
        {
            n->Value = input.ReadInt8();
        }
        else
        {
            q.push(n->LeftChild);
            q.push(n->RightChild);
        }
    }

    return t;
}

std::ostream& operator<<(std::ostream& out, const Tree& t)
{
    typedef Tree::NodePtr NodePtr;
    typedef std::pair<uint, NodePtr> QElem;
    std::queue<QElem> q;

    q.push(std::make_pair(0, t.Root));
    uint level = 0;
    while(!q.empty())
    {
        QElem node = q.front();
        q.pop();

        if (!node.second) continue;

        if (node.first > level)
        {
            level = node.first;
            out << std::endl;
        }

        if (node.second->IsLeaf)
        {
            out << "( " << node.second->Value << ", " << node.second->Frequency << " )";
        }
        else
        {
            out << "-";
            q.push(std::make_pair(node.first + 1, node.second->LeftChild));
            q.push(std::make_pair(node.first + 1, node.second->RightChild));
        }
    }

    return out;
}
