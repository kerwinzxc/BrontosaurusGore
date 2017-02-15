#pragma once
#include "stack.h"
#include <assert.h>
#include <iostream>
#include <math.h>

namespace CU{

	template<typename T>
	class BinaryTree
	{
	public:

		class TreeNode
		{
		public:
			TreeNode(){ parent = nullptr; leftChild = nullptr; rightChild = nullptr; }
			TreeNode(T aValue, TreeNode* aParent){value = aValue; parent = aParent; }
	
			
			T value;
			TreeNode* parent;


			TreeNode* leftChild;
			TreeNode* rightChild;

			void SetLeftChild(TreeNode* aSubTree)
			{
				if (aSubTree != nullptr)
				{
					aSubTree->parent = this;
				}
				leftChild = aSubTree;
			}

			void SetRightChild(TreeNode* aSubTree)
			{
				if (aSubTree != nullptr)
				{
					aSubTree->parent = this;
				}
				rightChild = aSubTree;
			}

			void ReplaceChildWith(TreeNode* aParent, TreeNode* aChild)
			{
				if (aParent == leftChild)
				{
					leftChild = aChild;
					aChild->parent = this;
				}
				else if (aParent == rightChild)
				{
					rightChild = aChild;
					aChild->parent = this;
				}
			}

		};

		BinaryTree()
		{
			myRoot = nullptr;
			mySize = 0;
		}

		void InsertNode(TreeNode* aNodeToInsert)
		{
			assert(aNodeToInsert != nullptr && "_____aNode is nullptr_____");
			++mySize;
			DeepInsert(aNodeToInsert, myRoot);
		}

		void InsertNode(T aValueToInsert)
		{
			TreeNode* treeNode;
			treeNode = new TreeNode();
			treeNode->value = aValueToInsert;

			if (myRoot == nullptr)
			{
				myRoot = treeNode;
				++mySize;
				return;
			}
			InsertNode(treeNode);

		}

		void DeleteNode(TreeNode* aNode) 
		{
			assert(aNode != nullptr && "_____Value not in tree._____");
			--mySize;
			DeleteNode(aNode->value);
		}

		int GetSize()
		{
			return mySize;
		}

		void DeleteNode(T aValue)
		{
			TreeNode* nodeToDelete = Find(aValue);
			assert(nodeToDelete != nullptr && "_____Value not in tree._____");

			if (nodeToDelete == myRoot)
			{
				TreeNode* tempLeft = nodeToDelete->leftChild;
				TreeNode* tempRight = nodeToDelete->rightChild;

				delete myRoot;
				myRoot = nullptr;

				if (tempLeft != nullptr)
				{
					InsertNode(tempLeft->value);
				}

				if (tempRight != nullptr)
				{
					InsertNode(tempRight->value);
				}
				return;
			}

			if (nodeToDelete == nodeToDelete->parent->leftChild)
			{
				nodeToDelete->parent->leftChild = nullptr;
			}
			else
			{
				nodeToDelete->parent->rightChild = nullptr;
			}

			if (nodeToDelete->leftChild != nullptr)
			{
				InsertNode(nodeToDelete->leftChild);
			}

			if (nodeToDelete->rightChild != nullptr)
			{
				InsertNode(nodeToDelete->rightChild);
			}

			delete nodeToDelete;
			nodeToDelete = nullptr;
		}

		TreeNode* GetRootNode() const
		{
			return myRoot;
		}

		TreeNode* Find(T aValue)
		{
			return DeepFind(myRoot, aValue);
		}
		
		void PreOrderTraverse()
		{
			Stack<TreeNode*> travStack;
			TreeNode* node = myRoot;

			if (node != nullptr)
			{
				travStack.Push(node);
				while (travStack.Size() > 0)
				{
					node = travStack.Pop();
					std::cout << node->value << " ";
					if (node->rightChild != nullptr)
					{
						travStack.Push(node->rightChild);
					}
					if (node->leftChild != nullptr)
					{
						travStack.Push(node->leftChild);
					}
				}
			}
			std::cout << std::endl;
		}

		void PostOrderTraverse()
		{
			Stack<TreeNode*> travStack;
			TreeNode *node = myRoot;
			TreeNode *secondNode = myRoot;

			while (node != nullptr)
			{
				for (; node->leftChild != nullptr; node = node->leftChild)
				{
					travStack.Push(node);
				}
				while (node->rightChild == nullptr || node->rightChild == secondNode)
				{ 
					std::cout << node->value << " ";
					secondNode = node;
					if (travStack.Size() == 0)
					{
						std::cout << std::endl;
						return;
					}
					node = travStack.Pop();
				}
				travStack.Push(node);
				node = node->rightChild;
			}
			std::cout << std::endl;
		}

		void InOrderTraverse()
		{
			Stack<TreeNode*> travStack;
			TreeNode* node = myRoot;

			while (node != nullptr)
			{
				while (node != nullptr)
				{
					if (node->rightChild != nullptr)
					{
						travStack.Push(node->rightChild);
					}
					travStack.Push(node);
					node = node->leftChild;
				}
				node = travStack.Pop();
				while (travStack.Size() > 0 && node->rightChild == nullptr)
				{
					std::cout << node->value << " ";
					node = travStack.Pop();
				}
				std::cout << node->value << " ";
				if (travStack.Size() > 0)
				{
					node = travStack.Pop();
				}
				else
				{
					node = nullptr;
				}
			}
			std::cout << std::endl;
		}

		void DSW_Balance()
		{
			CreateVine();
			CreatePerfectTree();
		}

		void CreatePerfectTree()
		{
			int n = mySize;
			int m = pow(2, static_cast<int>(log2(n + 1))) - 1;

			TreeNode* temp = myRoot->rightChild;

			for (int i = 0; i < n - m; ++i)
			{
				RotateLeft(temp->parent, temp, temp->rightChild);
				temp = temp->rightChild;
				if (temp != nullptr)
				{
					temp = temp->rightChild;
				}
			}

			while (m > 1)
			{
				m /= 2;
				temp = myRoot->rightChild;
				for (int i = 0; i < m; ++i)
				{
					RotateLeft(temp->parent, temp, temp->rightChild);
					temp = temp->rightChild;
					if (temp != nullptr)
					{
						temp = temp->rightChild;
					}
			/*		if (temp->parent == nullptr)
					{
						temp->parent = temp;
					}
					else
					{
						if (temp != nullptr)
						{
							temp->parent = temp;
						}
					}

					temp = temp->rightChild;*/
				}
			}
		}

		void CreateVine()
		{
			TreeNode* temp = myRoot;

			while (temp != nullptr)
			{ 
				if (temp->leftChild != nullptr)	 
				{
					RotateRight(temp->parent, temp, temp->leftChild);
					temp = temp->parent;
				}
				else
				{
					temp = temp->rightChild;
				}

			} 
		}

		void RotateRight(TreeNode* aGrandParent, TreeNode* aParent, TreeNode* aChild)
		{
			if (aGrandParent != nullptr)
			{
				aGrandParent->ReplaceChildWith(aParent, aChild);
				aParent->SetLeftChild(aChild->rightChild);
				aChild->SetRightChild(aParent);
			}
			else
			{
				myRoot = aChild;
				myRoot->parent = nullptr;
				aParent->SetLeftChild(aChild->rightChild);
				aChild->SetRightChild(aParent);
			}
		}

		void RotateLeft(TreeNode* aGrandParent, TreeNode* aParent, TreeNode* aChild)
		{
			if (aGrandParent == myRoot)
			{
				myRoot = aParent;
				myRoot->parent = nullptr;
			}

			TreeNode* parent = aGrandParent->parent;
			if (parent != nullptr)
			{
				parent->SetRightChild(aParent);
			}

			aGrandParent->parent = aParent;

			TreeNode* leftNode = aParent->leftChild;

			aParent->SetLeftChild(aGrandParent);
			aParent->parent = parent;

			aGrandParent->SetRightChild(leftNode);

			if (leftNode != nullptr)
			{
				leftNode->parent = aGrandParent;
			}
		}

	private:

		TreeNode* DeepFind(TreeNode* aTreeNode, const T aValue)
		{
			if (aTreeNode == nullptr)
			{
				return nullptr;
			}
			else if (aTreeNode->value == aValue)
			{
				return aTreeNode;
			}
			else if (aValue < aTreeNode->value && aTreeNode->leftChild != nullptr)
			{
				return DeepFind(aTreeNode->leftChild, aValue);
			}
			else if (aValue > aTreeNode->value && aTreeNode->rightChild != nullptr)
			{
				return DeepFind(aTreeNode->rightChild, aValue);
			}
			else
			{
				return nullptr;
			}
		}

		//pass in da Bas Rutten
		void DeepInsert(TreeNode* aTreeNodeToInsert, TreeNode* aTreeNodePos)
		{
			aTreeNodeToInsert->parent = aTreeNodePos;

			if (aTreeNodeToInsert->value < aTreeNodePos->value)
			{
				if (aTreeNodePos->leftChild == nullptr)
				{
					aTreeNodePos->leftChild = aTreeNodeToInsert;
					return;
				}
				
				DeepInsert(aTreeNodeToInsert, aTreeNodePos->leftChild);
				return;
			}
	
			if (aTreeNodeToInsert->value > aTreeNodePos->value)
			{
				if (aTreeNodePos->rightChild == nullptr)
				{
					aTreeNodePos->rightChild = aTreeNodeToInsert;
					return;
				}
				DeepInsert(aTreeNodeToInsert, aTreeNodePos->rightChild);
				return;
			}
		}

		int mySize;
		TreeNode* myRoot;
	};
} 


#pragma region secretKyleCode
#define KYLE 1337

class KyleDetector
{
	typedef int kyle;
public:
	KyleDetector();

	bool isKyle(kyle aKyle)
	{
		if (myKyle == KYLE)
		{
			return true;
		}
	}
private:
	kyle myKyle;
};

#pragma endregion secretKyleCode