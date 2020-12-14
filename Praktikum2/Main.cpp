#include "stdio.h"
#include "DateTime.h"
#include "Objects.h" 
#include "Headers.h"
#include "Structs.h"
#include "string.h"
#include "ctype.h"
#include "malloc.h"
#include "time.h"

#pragma warning( disable: 4996 )

#define OBJECT 9

// Tree visualizer

#include<iostream>

using namespace std;

class xTree;

class xNode {
public:
	xNode(int val) : _val(val) {}
	int val() { return _val; }
	void add(xNode* temp)
	{
		if (temp->val() > _val)
		{
			if (_rchild)
				_rchild->add(temp);
			else
			{
				_rchild = temp;
			}
		}
		else
		{
			if (_lchild)
				_lchild->add(temp);
			else
			{
				_lchild = temp;
			}
		}
	}
	void print()
	{
		for (int ix = 0; ix < _level; ++ix) cout << ' ';
		cout << _val << endl;
		++_level;
		if (_lchild)
		{
			_lchild->print();
			--_level;
		}
		if (_rchild)
		{
			_rchild->print();
			--_level;
		}
	}
private:
	int _val;
	xNode* _lchild;
	xNode* _rchild;
	static int _level;
};

int xNode::_level = 0;

class xTree {
public:
	xTree() : _root(0) {}
	void add(int val)
	{
		xNode* temp = new xNode(val);
		if (!_root)
			_root = temp;
		else
			_root->add(temp);
	}
	void print()
	{
		if (!_root)
			return;
		_root->print();
	}
private:
	xNode* _root;
};

xTree tree;


int Counter = 1;

/* PrintObjects */
void PrintObjects(HeaderC* pStruct4) {
	if (pStruct4 != NULL)
	{
		// Pointer to first element of top array
		HeaderC* pTemp = pStruct4;
		int counter = 1;

		Object9* obj = NULL;

		// Looping through top array
		while (pTemp != NULL)
		{
			// Get the array of pointers to elements from top array
			int* p = (int*)pTemp->ppObjects;

			// Now since I have the array of pointers to objects
			// I have to iterate through the pointers from A to Z
			for (int i = 0; i < 26; i++)
			{
				// Grabbing the pointer to object
				int* pToObject = (int*)p[i];

				if (pToObject != NULL)
				{
					// Getting the object
					obj = (Object9*)pToObject;

					// Looping through other objects connected to the object while they exist
					while (obj != NULL)
					{
						tree.add((int)obj->Code / 100000);
						printf("%d) %s %lu %02d %s %04d\n", counter, obj->pID, obj->Code, obj->pDate3->Day, obj->pDate3->pMonth, obj->pDate3->Year);
						counter++;
						obj = obj->pNext;
					}
				}

			}

			pTemp = pTemp->pNext;
		}
	}

	return;
}

/* GetSecondCapital
 * Finds whitespace in string
 * and moves to the next char */
char GetSecondCapital(char* pNewID)
{
	char* scndLtr = strchr(pNewID, ' ');

	if (scndLtr == NULL)
		return 0;

	scndLtr++;
	return *scndLtr;
}

/* New ID validator */
bool CheckNewID(char* pNewID)
{
	// Check for space in the new ID
	char* space = strchr(pNewID, ' ');
	if (space == NULL)
		return 0;
	
	// Check if both IDs start with capitals
	if (!isupper(pNewID[0]) || !isupper(GetSecondCapital(pNewID)))
		return 0;


	// Checks passed
	return 1;
}

/* Checks if header contains any other objects 
 * Returns false when no other objects found
 * Returns true if any other object exists    */
bool HeaderChecker(void** arr)
{
	if (arr == NULL)
		return false;

	for (int i = 0; i < 26; i++)
	{
		if (arr[i] != NULL) return true;
	}

	return false;
}

/* NewObject
 * Creates a new object to be inserted
 * Returns NULL if no memory could be allocated
 * Returns created object */
Object9* NewObject(char* pID, unsigned long int Code)
{
	// Allocating memory for object and its variables
	Object9* obj = (Object9*)malloc(sizeof(Object9));
	Date3* date = (Date3*)malloc(sizeof(Date3));
	char* dateBuf = (char*)malloc(sizeof(double) * 2);
	char* nameBuf = (char*)malloc(strlen(pID));

	if (obj == NULL || date == NULL || dateBuf == NULL || nameBuf == NULL)
		return NULL;

	// Assigning object values
	strcpy(nameBuf, pID);
	obj->pID = nameBuf;
	obj->Code = Code;
	obj->pNext = NULL;

	// Getting object 
	time_t dateTime;
	int getDate = GetDate3(time_t(time(&dateTime)), (sizeof(double) * 2), dateBuf, date);

	// If getting date failed
	if (getDate != 1)
		return NULL;

	date->pMonth = dateBuf;
	obj->pDate3 = date;

	// Return the new object
	return obj;
}

/* InsertNewObject
 * Returns 1 if succeeded
 * Returns 0 if in case of failure */
int InsertNewObject(HeaderC** pStruct4, char* pNewID, int NewCode)
{
	// Checking inputs
	if (pStruct4 == NULL || pNewID == NULL || NewCode < 0)
		return 0;

	// Checking if NewID matches format
	if (!CheckNewID(pNewID))
	{
		printf("\n%s - False id!\n", pNewID);
		return 0;
	}
		

	// Finding the place in the upper struct
	HeaderC* pStruct = *pStruct4;

	// Counter to check if new link has to be created as a first link
	int counter = 0;

	while (pStruct != NULL)
	{
		// If top struct beginning letter matches with new ID first letter
		if (pStruct->cBegin == pNewID[0])
		{
			// Variables for finding second pointer
			int* p = (int*)pStruct->ppObjects;
			char c = 'A';

			// Looping from A - Z to find matching second letter
			for (int i = 0; i < 26; i++, c++)
			{
				int* pToObject = (int*)p[i];

				// Check if in the second letters position
				if (c == GetSecondCapital(pNewID))
				{
					// Check if object exists
					if (pToObject != NULL)
					{
						Object9* obj = (Object9*)pToObject;

						// Loop until last object
						do
						{
							// Checking if object ID already exists
							if (!strcmp(obj->pID, pNewID))
								return 0;

							// Check if not leading into nullptr
							if (obj->pNext == NULL) 
								break;

							obj = obj->pNext;

						} while (obj->pNext != NULL || obj != NULL);

						// Object did not exist therefore adding a new object
						printf("\n--- Adding object ---\n\n");
						Object9* newObj = NewObject(pNewID, NewCode);

						// Check if new object was successfully created
						if (newObj == NULL)
							return 0;

						// Add a pointer to next object
						obj->pNext = newObj;

						return 1;
					}
					else
					{
						// Create the object and add it to the list
						printf("\n--- Adding NEW object ---\n\n");
						Object9* newObj = NewObject(pNewID, NewCode);
						p[i] = (int)newObj;

						return 1;
					}
				}
			}

		}
		else if ((pStruct->pNext == NULL) || (pStruct->pNext->cBegin > pNewID[0]))
		{
			// Creating a top array link
			HeaderC* newLink = (HeaderC*)malloc(sizeof(HeaderC));
			int* pObjArr = (int*)malloc(sizeof(int*) * 26);

			if (pObjArr == NULL)
				return 0;

			memset(pObjArr, NULL, sizeof(int*) * 26);

			newLink->cBegin = pNewID[0];
			newLink->pNext = NULL;
			newLink->ppObjects = (void**)pObjArr;

			// If new first link
			if (counter == 0)
			{
				HeaderC* tempH = *pStruct4;
				*pStruct4 = newLink;
				newLink->pNext = tempH;
			}
			else
			{
				newLink->pNext = pStruct->pNext;
				pStruct->pNext = newLink;
			}

			pStruct = newLink;
		}
		else 
		{
			pStruct = pStruct->pNext;
		}

		// Increment loop counter
		counter++;
		
	}


	return 1;
}

/* Remove Object */
Object9* RemoveExistingObject(HeaderC** pStruct4, char* pExistingID)
{
	// Checking if parameters arent NULL and if such ID can exist
	if (pStruct4 == NULL || pExistingID == NULL)
		return NULL;

	if (!CheckNewID(pExistingID))
	{
		printf("\n%s - ID invalid!\n", pExistingID);
		return 0;
	}

	// Finding the place in the upper struct
	HeaderC* pStruct = *pStruct4;

	// Looping through the upper struct
	while (pStruct != NULL)
	{
		// Checking if the first letter of ID matches
		if (pStruct->cBegin == pExistingID[0])
		{
			// Variables for finding the pointer to objects
			int i;
			char c = 'A';
			char sC = GetSecondCapital(pExistingID);

			// Getting the index of second capital
			for (i = 0; c != sC; i++, c++);

			// Getting the pointer to object of index
			int* pToObject = (int*)pStruct->ppObjects[i];

			// Getting the first object
			Object9* obj = (Object9*)pToObject;

			// Looping through the objects until correct obj is found
			while (obj != NULL)
			{
				// Match found, removing the links
				if (!strcmp(obj->pID, pExistingID))
				{
					// Remembering the old pointer to switch with object before
					Object9* objTemp = obj->pNext;

					// Object before needs to point to objTemp
					// Getting object before
					Object9* objBefore = (Object9*)pToObject;

					// Looping until there are objects to loop and at the object before
					while (objBefore != NULL)
					{
						// Reassigning the next object if at the object before
						if (objBefore->pNext == obj)
						{
							objBefore->pNext = objTemp;

							// Returning the pointer to the unlinked object
							return obj;
						}
						// Checking if first pointer to object
						else if (objBefore == obj)
						{
							// Changing the first pointer to NULL
							pStruct->ppObjects[i] = NULL;

							// Check if need to remove header
							if (!HeaderChecker(pStruct->ppObjects))
							{
								HeaderC* tempHeader = *pStruct4;

								// Not sure why this works
								while (tempHeader != NULL)
								{
									if (tempHeader->pNext == pStruct)
									{
										// Removes link in between two top array links
										tempHeader->pNext = pStruct->pNext;
										free(pStruct);
										break;
									}
									else if (tempHeader == pStruct)
									{
										// Removes the first link in top array
										*pStruct4 = pStruct->pNext;
										free(pStruct);
										break;
									}

									tempHeader = tempHeader->pNext;
								}
							}

							// Returning the pointer to the unlinked object
							return obj;
						}

						objBefore = objBefore->pNext;
					}
				}

				obj = obj->pNext;
			}

			// Exit the while loop
			break;
		}

		pStruct = pStruct->pNext;
	}

	return NULL;
}

/* Funktsioonid 4-6 */
void pProcess(Node* p)
{
	printf("%3d) %20s %lu %02d %s %04d\n",Counter, ((Object9*)p->pObject)->pID, ((Object9*)p->pObject)->Code, ((Object9*)p->pObject)->pDate3->Day, ((Object9*)p->pObject)->pDate3->pMonth, ((Object9*)p->pObject)->pDate3->Year);
	Counter++;
}

// CompareNodes (CreateBinaryTree helper function)
int CompareNodes(Node* top, Node* p) {
	// Get objects from the Nodes
	Object9* obj = (Object9 *)top->pObject;
	Object9* temp = (Object9*)p->pObject;

	// If top code is bigger than new link code return 0
	if (obj->Code > temp->Code) {
		return 0;
	}

	// Else return 1
	return 1;
}

Node* CreateBinaryTree(HeaderC* pStruct4) {

	// Tree top
	Node* pTopNode = (Node*)malloc(sizeof(Node));

	if (pStruct4 != NULL)
	{
		// Pointer to first element of top array
		HeaderC* pTemp = pStruct4;
		int counter = 1;

		Object9* obj = NULL;

		// Looping through top array
		while (pTemp != NULL)
		{
			// Get the array of pointers to elements from top array
			int* p = (int*)pTemp->ppObjects;

			// Now since I have the array of pointers to objects
			// I have to iterate through the pointers from A to Z
			for (int i = 0; i < 26; i++)
			{
				// Grabbing the pointer to object
				int* pToObject = (int*)p[i];

				if (pToObject != NULL)
				{
					// Getting the object
					obj = (Object9*)pToObject;

					// Looping through other objects connected to the object while they exist
					while (obj != NULL)
					{
						// Creating the topmost element of the tree
						if (counter == 1)
						{
							pTopNode->pObject = obj;
							pTopNode->pLeft = pTopNode->pRight = 0;
						}
						else
						{
							// New link in tree
							Node* pNew = (Node *)malloc(sizeof(Node));
							pNew->pObject = obj;
							pNew->pLeft = pNew->pRight = 0;

							// Adding link to tree
							for (Node* p = pTopNode; 1;) {
								if (CompareNodes(p, pNew) == 0) {
									if (!p->pLeft) {
										p->pLeft = pNew;
										break; // Exit loop after adding link
									}
									else
										p = p->pLeft; // Move one link to left
								}
								else {
									if (!p->pRight) {
										p->pRight = pNew;
										break; // Exit loop after adding link
									}
									else
										p = p->pRight; // Move one link to right
								}
							}
						}
						counter++;
						obj = obj->pNext;
					}
				}

			}

			pTemp = pTemp->pNext;
		}
	}

	return pTopNode;
}

Stack* Push(Stack* pStack, void* pObject)
{
	errno = 0;
	Stack* pNew;
	if (!pObject)
	{
		errno = EINVAL;
		return pStack;
	}

	pNew = (Stack*)malloc(sizeof(Stack));
	pNew->pObject = pObject;
	pNew->pNext = pStack;

	return pNew;
}

Stack* Pop(Stack* pStack, void** pObject)
{
	Stack* p;
	if (!pStack)
	{
		*pObject = 0;
		return pStack;
	}
	*pObject = pStack->pObject;
	p = pStack->pNext;
	free(pStack);
	return p;
}

void TreeTraversal(Node* pTree) {
	Stack* pStack = 0;
	Node* p1 = pTree, *p2;
	if (!pTree)
		return;
	do {
		while (p1)
		{
			pStack = Push(pStack, p1);
			p1 = p1->pLeft;
		}
		pStack = Pop(pStack, (void**)&p2);
		(pProcess)(p2);
		p1 = p2->pRight;
	} while (!(!pStack && !p1));
}

Node* DeleteTreeNode(Node* pTree, unsigned long int Code)
{
	Node* pBefore = NULL;

	for (Node* p = pTree; 1;)
	{
		if (p == NULL) {
			printf("\nNo match found to delete!\n");
			return NULL;
		}

		// Found matching node
		if (((Object9*)p->pObject)->Code == Code)
		{
			// If node without leaves
			if (p->pLeft == NULL && p->pRight == NULL)
			{
				// Remove pointer to this leaf
				if (pBefore->pLeft = p)
					pBefore->pLeft = NULL;
				else 
					pBefore->pRight = NULL;

				return p;
			}

			// If node has two leaves
			if (p->pLeft != NULL && p->pRight != NULL)
			{
				Node* pTemp = NULL;

				for (pTemp = p->pRight; pTemp->pLeft != NULL; pTemp = pTemp->pLeft);
				pTemp->pLeft = p->pLeft;

				// If is the first node of the whole tree
				if (p == pTree)
					*pTree = *p->pRight;

				// If exists a node before
				if (pBefore != NULL)
				{
					if (CompareNodes(pBefore, p) == 0)
						pBefore->pLeft = p->pRight;
					else
						pBefore->pRight = p->pRight;
				}

				return p;
			}

			// Node has a single leaf

			// Checking which leaf exists
			// Just assigning the next leaf to the current nodes place
			if (p->pLeft != NULL)
			{
				if (CompareNodes(pBefore, p) == 0)
					pBefore->pLeft = p->pLeft;
				else
					pBefore->pRight = p->pLeft;

				return p;
			}

			if (p->pRight != NULL)
			{
				if (CompareNodes(pBefore, p) == 0)
					pBefore->pLeft = p->pRight;
				else
					pBefore->pRight = p->pRight;

				return p;
			}
			

		}

		// If not matching node

		// Remember the node before for easier removal
		pBefore = p;

		// Compare node leaves and move to next leaf
		if (((Object9*)p->pObject)->Code > Code)
			p = p->pLeft;
		else
			p = p->pRight;
	}

	printf("\nNo match found to delete!\n");
	return NULL;
}

int main()
{
	// Genereerin andmed
	HeaderC* pStruct = GetStruct4(OBJECT, 10);
	//RemoveExistingObject(&pStruct, (char*)"Gtmqw Scjbeiltp");
	PrintObjects(pStruct);

	/*
	// Test objects
	InsertNewObject(&pStruct, (char*)"Dx Gz", 77777777);
	InsertNewObject(&pStruct, (char*)"Dx Ga", 77777777);
	InsertNewObject(&pStruct, (char*)"Db Aa", 77777777);
	InsertNewObject(&pStruct, (char*)"Dk Za", 77777777);
	InsertNewObject(&pStruct, (char*)"Dr Wa", 77777777);
	InsertNewObject(&pStruct, (char*)"Aa Aa", 77777777);
	InsertNewObject(&pStruct, (char*)"Ab Ba", 77777777);
	InsertNewObject(&pStruct, (char*)"Za Aa", 77777777);
	InsertNewObject(&pStruct, (char*)"Za Ab", 77777777);
	InsertNewObject(&pStruct, (char*)"Za Ba", 77777777);
	InsertNewObject(&pStruct, (char*)"Wx Aa", 77777777);

	// Should report error
	InsertNewObject(&pStruct, (char*)"zb Kk", 77777777);
	InsertNewObject(&pStruct, (char*)"Zc ca", 77777777);
	InsertNewObject(&pStruct, (char*)"Dr  Wa", 77777777);
	InsertNewObject(&pStruct, (char*)"ZB kk", 77777777);
	InsertNewObject(&pStruct, (char*)"Fa", 77777777);
	InsertNewObject(&pStruct, (char*)"Fa_Fa", 77777777);

	printf("\n-- ADDITION RESULTS --\n");
	PrintObjects(pStruct);

	// Test objects
	RemoveExistingObject(&pStruct, (char*)"Dx Gz");
	RemoveExistingObject(&pStruct, (char*)"Dx Gz");
	RemoveExistingObject(&pStruct, (char*)"Dx Gz");
	RemoveExistingObject(&pStruct, (char*)"Dx Ga");
	RemoveExistingObject(&pStruct, (char*)"Db Aa");
	RemoveExistingObject(&pStruct, (char*)"Dk Za");
	RemoveExistingObject(&pStruct, (char*)"Dr Wa");
	RemoveExistingObject(&pStruct, (char*)"Aa Aa");
	RemoveExistingObject(&pStruct, (char*)"Ab Ba");
	RemoveExistingObject(&pStruct, (char*)"Za Aa");
	RemoveExistingObject(&pStruct, (char*)"Za Ab");
	RemoveExistingObject(&pStruct, (char*)"Za Ba");
	RemoveExistingObject(&pStruct, (char*)"Wx Aa");

	// Should report error
	RemoveExistingObject(&pStruct, (char*)"zb Kk");
	RemoveExistingObject(&pStruct, (char*)"Zc ca");
	RemoveExistingObject(&pStruct, (char*)"Dr  Wa");
	RemoveExistingObject(&pStruct, (char*)"ZB kk");
	RemoveExistingObject(&pStruct, (char*)"Fa");
	RemoveExistingObject(&pStruct, (char*)"Fa_Fa");
	
	printf("\n-- REMOVAL RESULTS --\n");
	PrintObjects(pStruct);
	*/

	printf("\n");
	Node* pTree = CreateBinaryTree(pStruct);
	//TreeTraversal(pTree);

	printf("\n");
	//Counter = 1;

	DeleteTreeNode(pTree, 316985719);

	TreeTraversal(pTree);

	tree.print();

	return 0;
}