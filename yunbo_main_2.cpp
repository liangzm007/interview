#include <iostream>
#include <list>
#include <string>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <algorithm>

struct ListNode {
    int val;
    ListNode *next;

    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

class Solution
{
public:
    ListNode* removeSublists(ListNode* head)
    {
        const int threshold = 10;

        ListNode* front = nullptr;
        for (ListNode* begin = head; begin != nullptr;)
        {
            int result = begin->val;
            ListNode* current = begin;
            bool find = false;
            for (ListNode* end = begin->next; end != nullptr; end = end->next)
            {
                result *= end->val;
                if (result == threshold)
                {
                    find = true;
                    current = end;
                    continue;
                }

                if (find)
                {
                    if (front)
                    {
                        front->next = current->next;
                    }
                    else
                    {
                        head = current->next;
                    }

                    break;
                }
            }

            if (find)
            {
                begin = head;
                front = nullptr;
            }
            else
            {
                front = begin;
                begin = begin->next;
            }
        }

        return head;
    }
};

int main()
{
    ListNode node13(99);
    ListNode node12(5,  &node13);
    ListNode node11(5,  &node12);
    ListNode node1(2,   &node11);
    ListNode node2(2,   &node1);
    ListNode node3(2,   &node2);
    ListNode node4(10,  &node3);
    ListNode node5(-1,  &node4);
    ListNode node6(1,   &node5);
    ListNode node7(-10, &node6);
    ListNode node8(-1,  &node7);
    ListNode node9(5,   &node8);
    ListNode node10(-2, &node9);

    Solution s;
    ListNode* result = s.removeSublists(&node10);

    return 0;
}
