#ifndef TREE_H_
#define TREE_H_

#include "bytebuffer.h"

#include <deque>
#include <ostream>

//! Binary tree data structure
/*!
    Used in Huffman compression algorithm
*/
struct Tree
{
    //! Node Structure
    /*!
        Represents a node in the binary tree
    */
    struct Node
    {
        typedef Node* NodePtr;

        Node(); ///< Constructor used to create a none leaf node
        Node(char val, uint freq); ///< Constructor used to create a leaf node of the tree


        bool IsLeaf;
        NodePtr Parent;

        /// Leafs contain characters and their frequency, nodes contain pointers to their sons
        union
        {
            struct
            {
                NodePtr LeftChild;
                NodePtr RightChild;
            };

            struct
            {
                char Value;
                uint Frequency;
            };
        };
    };

    typedef Node::NodePtr NodePtr;

    Tree(); ///< Default Constructor
    Tree(Node* n); ///< Constructor

    NodePtr Root; ///< Root Node

    uint32 CalcWeight() const; ///< Returns the summation of the frequencies of the tree
    std::deque<bool> Find(char val) const; ///< Returns the Huffman code of the given character
    void DeleteAll(); ///< Deletes every node in the tree
    void Save(ByteBuffer& output); ///< Saves the structure to the given BybteBuffer
    static Tree Load(ByteBuffer& input); ///< Returns a Tree loaded from the given ByteBuffer
};

std::ostream& operator <<(std::ostream& out, const Tree& t);

#endif // TREE_H_
