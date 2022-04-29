#include <stdio.h>
#include <stdlib.h>

// Make a short name for a Node.
typedef struct Node Node;

// Node to build our list from.
struct Node {
  // Value in our list
  int val;
  
  // Pointer to the next node.
  Node *next;
};

// Representation for a list, with a head and a tail pointer.
typedef struct {
  // Head pointer for the list.
  Node *head;
  
  // Pointer to the last node in the list, or null if the list
  // is empty.
  Node *tail;
} List;

struct Node *parition(struct Node *first, struct Node *last)
{
	//Get first node of given linked list
	struct Node *pivot = first;
	struct Node *front = first;
	int temp = 0;
	while (front != NULL && front != last)
	{
		if (front->val < last->val)
		{
			pivot = first;
			//Swap node value
			temp = first->val;
			first->val = front->val;
			front->val = temp;
			//Visit to next node
			first = first->next;
		}
		//Visit to next node
		front = front->next;
	}
	//Change last node value to current node
	temp = first->val;
	first->val = last->val;
	last->val = temp;
	return pivot;
}
 

//Perform quick sort in given linked list
void quick_sort(struct Node *first, struct Node *last)
{
	if (first == last)
	{
		return;
	}
	struct Node *pivot = parition(first, last);
	if (pivot != NULL && pivot->next != NULL)
	{
		quick_sort(pivot->next, last);
	}
	if (pivot != NULL && first != pivot)
	{
		quick_sort(first, pivot);
	}
}

// Recursive quicksort.
void sort( List *list ) {
  quick_sort(list->head, list->tail);
}

int main( int argc, char *argv[] )
{
  int n = 10;
  if ( argc >= 2 )
    if ( argc > 2 || sscanf( argv[ 1 ], "%d", &n ) != 1 ||
         n < 0 ) {
      fprintf( stderr, "usage: array <n>\n" );
      exit( EXIT_FAILURE );
    }
  
  // Representaton for the list.
  List list = { NULL, NULL };

  // Pointer to the null pointer at the end of our list, so we can grow
  // the list front-to-back;
  for ( int i = 0; i < n; i++ ) {
    // Make a node containing a random value.
    Node *n = (Node *) malloc( sizeof( struct Node ) );
    *n = (Node) { rand(), NULL };

    // Link it in at the tail end of the list.
    if ( list.tail )
      list.tail->next = n;
    else
      list.head = n;
    list.tail = n;
  }

  sort( &list );

  // Print the sorted items.
  for ( Node *n = list.head; n; n = n->next )
    printf( "%d\n", n->val );

  // Free memory for the list.
  while ( list.head ) {
    Node *n = list.head;
    list.head = n->next;
    free( n );
  }

  return 0;
}
