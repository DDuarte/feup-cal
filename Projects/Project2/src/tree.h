#ifndef TREE_H_
#define TREE_H_

#include <utility>
#include <memory>
#include <stack>
#include <queue>
#include <ostream>
#include <sstream>
#include <tuple>

struct Tree
{
    struct Node
    {
        typedef Node* NodePtr;

        Node(char val, uint freq) : Value(val), Frequency(freq), IsLeaf(true) { }
        Node() : IsLeaf(false), LeftChild(nullptr), RightChild(nullptr) { }

        bool IsLeaf;
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

    Tree() : Root(nullptr) { }
    Tree(Node* n) : Root(n) { }

    NodePtr Root;

    uint32 CalcWeight() const
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

    std::deque<bool> Find(char val) const
    {
        typedef std::tuple<char, bool, NodePtr> AuxElem;
        std::stack<AuxElem> stk;
        stk.push(std::make_tuple(' ', false, Root));

        while(!stk.empty())
        {
            AuxElem aux = stk.top();
            NodePtr node = std::get<2>(aux);


            if (!node)
            {
                stk.pop();
                continue;
            }

            if (std::get<1>(aux))
            {
                stk.pop();
                continue;
            }

            if (node->IsLeaf)
            {
                if (node->Value == val)
                {
                    stk.pop();
                    stk.push(std::make_tuple(std::get<0>(aux), true, node));
                    break;
                }
                else
                    stk.pop();
            }
            else
            {
                stk.pop();
                stk.push(std::make_tuple(std::get<0>(aux), true, node));
                stk.push(std::make_tuple('0', false, node->LeftChild));
                stk.push(std::make_tuple('1', false, node->RightChild));
            }
        }

        std::deque<bool> result;

        while (!stk.empty())
        {
            AuxElem aux = stk.top();
            stk.pop();
            if (std::get<1>(aux) && std::get<0>(aux) != ' ')
                result.push_front(std::get<0>(aux) == '1');
        }

        return result;
    }

    void DeleteAll()
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
};

std::ostream& operator <<(std::ostream& out, const Tree& t)
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

#endif // TREE_H_
