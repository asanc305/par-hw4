void Enqueue(int);
void Dequeue();

struct Node {
	int data;
	struct Node* next;
};

struct Data {
  int id;
  int min;
  int max;
  int k;
  pthread_cond_t *noWork;
  pthread_mutex_t *infoLock;
};

struct Node* front = NULL;
struct Node* rear = NULL;

void Enqueue(int x) {
	struct Node* temp = 
		(struct Node*)malloc(sizeof(struct Node));
	temp->data =x; 
	temp->next = NULL;
	if(front == NULL && rear == NULL){
		front = rear = temp;
		return;
	}
	rear->next = temp;
	rear = temp;
}

void Dequeue() {
	struct Node* temp = front;
	if(front == NULL) {
		printf("Queue is Empty\n");
		return;
	}
	if(front == rear) {
		front = rear = NULL;
	}
	else {
		front = front->next;
	}
	free(temp);
}
