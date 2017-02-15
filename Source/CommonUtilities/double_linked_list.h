#pragma once

namespace CU
{
	template<typename T>
	class DoubleLinkedList
	{
	public:

		class Node
		{
		public:
			Node(const T &aValue)
			{
				value = aValue;
				next = nullptr;
				prev = nullptr;
			}
			T value;

			Node* next;
			Node* prev;
		};

		DoubleLinkedList()
		{
			myLast = myFirst = nullptr;
			mySize = 0;
		}


		void AddNode(Node *aNode)
		{
			if (myFirst == nullptr)
			{
				myLast = myFirst = aNode;
			}
			else
			{
				myLast->next = aNode;
				aNode->next = nullptr;
				aNode->prev = myLast;
				myLast = aNode;
			}
			++mySize;
		}


		/**
		*  Returns the tail of the list.
		*
		*  @return Returns the first node of the list or nullptr if the list
		*          is empty.
		*/
		Node *GetFirstNode()
		{
			return myFirst;
		}


		/**
		*  Returns the head of the list.
		*
		*  @return Returns the last node of the list or nullptr if the list
		*          is empty.
		*/
		Node *GetLastNode(void)
		{
			return myLast;
		}


		/**
		*  Inserts a node before another node.
		*
		*  @param  aNode gives the node which to place the new node before
		*  @param  aNodeToInsert gives the node to insert into the list
		*/
		void InsertNodeBeforeNode(Node *aNode, Node *aNodeToInsert)
		{
			aNodeToInsert->prev = aNode->prev;
			aNodeToInsert->next = aNode;

			if (aNode->prev != nullptr)
			{
				aNode->prev->next = aNodeToInsert;
			}
			else
			{
				myFirst = aNodeToInsert;
			}
			aNode->prev = aNodeToInsert;
			++mySize;
		}


		/**
		*  Insert a node sorted in decending order. The value of the node to
		*  insert must be comparable using the < operator.
		*
		*  @param  aNodeToInsert gives the node to insert.
		*/
		void InsertNodeSorted(Node *aNodeToInsert)
		{
			if (mySize > 0)
			{
				for (Node* currentNode = GetFirstNode(); currentNode != nullptr; currentNode->next)
				{
					if (aNodeToInsert->value < currentNode->value)
					{
						currentNode = currentNode->next;
						continue;
					}
					InsertNodeBeforeNode(currentNode, aNodeToInsert);
					return;
				}
			}
			AddNode(aNodeToInsert);
		}


		/**
		*  Insert a node sorted in decending order. The value to insert must
		*  be comparable using the < operator.
		*
		*  @param  aNodeToInsert gives the node to insert.
		*/
		void InsertNodeSorted(const T &aValueToInsert)
		{
			Node* node = new Node(aValueToInsert);
			InsertNodeSorted(node);
		}


		/**
		*  Deletes the given node from the list.
		*
		*  !!Asserts if the node isn't found.
		*
		*  @param  aNodeToDelete gives the node to delete from the list.
		*/
		void DeleteNode(Node *aNodeToDelete)
		{
			for (Node* currentNode = GetFirstNode(); currentNode != nullptr; currentNode = currentNode->next)
			{
				if (currentNode == aNodeToDelete)
				{
					if (currentNode->next != nullptr)
					{
						if (currentNode == myFirst)
						{
							currentNode->next->prev = nullptr;
							myFirst = currentNode->next;
						}
						else
						{
							currentNode->prev->next = currentNode->next;
							currentNode->next->prev = currentNode->prev;
						}
					}
					else
					{
						myLast = currentNode->prev;
						currentNode->prev->next = nullptr;
					}

					delete currentNode;
					currentNode = nullptr;

					--mySize;
					return;
				}
			}
		}


		/**
		*  Deletes the first node with the given value from the list.
		*
		*  !!Asserts if the value isn't found.
		*
		*  @param  aValueToDelete gives the value to delete from the list.
		*/
		void DeleteNode(const T &aValueToDelete)
		{
			for (Node* currentNode = GetFirstNode(); currentNode != nullptr; currentNode = currentNode->next)
			{
				if (currentNode->value == aValueToDelete)
				{
					if (myFirst != myLast)
					{
						if (currentNode->next != nullptr)
						{
							if (currentNode == myFirst)
							{
								currentNode->next->prev = nullptr;
								myFirst = currentNode->next;
							}
							else
							{
								currentNode->prev->next = currentNode->next;
								currentNode->next->prev = currentNode->prev;
							}
						}
						else
						{
							myLast = currentNode->prev;
						}
					}

					delete currentNode;
					currentNode = nullptr;
					--mySize;

					if (mySize <= 0)
					{
						myFirst = nullptr;
						myLast = nullptr;
					}

					return;
				}
			}
		}


		/**
		*  Searches the list for the given value and if found returns the
		*  first node with the value.
		*
		*  @param  aValue gives the value to search for
		*
		*  @return Returns the first node with the given value or nullptr if
		*          the value isn't found.
		*/
		Node *Find(const T &aValue)
		{
			for (Node* currentNode = GetFirstNode(); currentNode != nullptr; currentNode = currentNode->next)
			{
				if (currentNode->value == aValue)
				{
					return currentNode;
				}
			}
			return nullptr;
		}


	private:
		int mySize;
		Node *myFirst, *myLast;
	};
}