#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct node {
    char c;
    int freq;
    struct node* left;
    struct node* right;
};

struct heap {
    struct node** arr;
    int heap_len;
    int arr_len;
    int size;
};

struct map{
    char c;
    char* code;
};

struct node* allocate_node(int freq){
    struct node* n=(struct node*)malloc(sizeof(struct node));
    n->freq=freq;
    n->left=n->right=NULL;
    return n;
} 

struct heap* create(int size, struct node** a) {
    struct heap *h = malloc(sizeof(struct heap));
    h->arr = a;
    h->heap_len = 0;
    h->arr_len = 0;
    h->size = size;
    return h;
}


void change(struct node** a,struct node** b){
    struct node* tmp=*a;
    *a=*b;
    *b=tmp;
}

void heapify(struct node* arr[], int n, int i) {
    int left = 2 * i;
    int right = left + 1;
    int small = i;
    if (left <= n && arr[small]->freq > arr[left]->freq) {
        small = left;
    }
    if (right <= n && arr[small]->freq > arr[right]->freq) {
        small = right;
    }
    if (small != i) {
        change(&arr[i], &arr[small]);
        heapify(arr, n, small);
    }
}
void min_heapify(struct heap *H, int i) {
    heapify(H->arr, H->heap_len, i);
}

void build(struct heap *H) {
    int i;
    int n = H->arr_len;
    H->heap_len = H->arr_len;
    for (i = n / 2; i >= 1; i--) {
        min_heapify(H, i);
    }
}

struct node* extract(struct heap *H){
    struct node* minVal = H->arr[1];
    H->arr[1]=H->arr[H->heap_len];
    H->heap_len-=1;
    min_heapify(H,1);
    return minVal;
}

void insert(struct node* arr[], int idx) {
    if (idx == 1) {
        return;
    }
    int parent = idx / 2;
    if (arr[parent]->freq > arr[idx]->freq) {
        change(&arr[parent], &arr[idx]);
        insert(arr, parent);
    }
}
void insert_main(struct heap *H, struct node* val) {
    int n = H->heap_len;
    H->size+=1;
    H->arr=realloc(H->arr,(H->size+1)*sizeof(struct node*));
    H->arr_len+=1;
    H->arr[n + 1] = val;
    insert(H->arr, n+1);
    H->heap_len += 1;
}
struct node* encoding(struct heap *h){
    int n=h->heap_len;
    build(h);
    for (int i = 1;i<n;i++){
        struct node* z = allocate_node(0);
        z->left = extract(h);
        z->right = extract(h);
        z->freq = z->left->freq + z->right->freq;
        insert_main(h, z);
    }
    return extract(h);
}

static int gIndx=1;
void mapping(struct node* root,struct map res[],char num[],char* startLoc){
    if (!root) return;

    if(root->left==NULL && root->right==NULL){
        struct map e;
        e.c=root->c;
        e.code=strdup(startLoc);
	e.code[num-startLoc] = '\0';
        res[gIndx]=e;
        gIndx+=1;
        return;
    }
    num[0]='0';
    mapping(root->left,res,num+1,startLoc);
    num[0]='1';
    mapping(root->right,res,num+1,startLoc);
}

struct node* mapdec(char res[],int n){
    int i=0;
    struct node* root=allocate_node(0);
    struct node* tmp=root;
    while(i<n){
        while(res[i]=='0' || res[i]=='1') {
            if(res[i]=='0'){
                if(tmp->left==NULL)
                    tmp->left=allocate_node(0);
                tmp=tmp->left;
            }
            else if(res[i]=='1'){
                if(tmp->right==NULL)
                    tmp->right=allocate_node(0);
                tmp=tmp->right;
            }
            i+=1;
        }
        tmp->c=res[i];
        tmp=root;
        i+=1;
    }
    return root;
}

char* decoding(struct node* root,char* in,FILE* out){
    if (root == NULL) {
        return NULL;
    }
    if(root->left==NULL && root->right==NULL){
        fprintf(out,"%c",root->c);
        return in;
    }
    if(in[0]=='0')
        return decoding(root->left,in+1,out);
    else
        return decoding(root->right,in+1,out);

}

int main(){
    printf("Enter the name of the input file\n");
    char fi[30];
    scanf("%s",fi);
    FILE* inp;
    inp=fopen(fi,"r");
    printf("Enter the name of the output file\n");
    char fo[30];
    scanf("%s",fo);
    FILE* out;
    printf("Enter mode (0 for compression, 1 for decompression)\n");
    int mode;
    scanf("%d",&mode);
    char read;
    if(mode==0){
        int size=0;
        int flag=1;
        struct node** a=malloc(sizeof(struct node*));
        while(fscanf(inp,"%c",&read)!=EOF){
	    if (isspace(read)) continue;
	    flag=1;
            for(int i=1;i<=size;i++){
                if(a[i]->c==read){
                    a[i]->freq+=1;
                    flag=0;
                    break;
                }
            }
            if(flag){
                size+=1;
                a=realloc(a,(size+1)*sizeof(struct node*));
                a[size]=allocate_node(1);
                a[size]->c=read;
            }
        }
        fclose(inp);
        struct heap* h=create(size,a);
        h->heap_len=size;
        h->arr_len=size;
        struct node* root=encoding(h);
        char code[30];
        gIndx=1;
        struct map* res=malloc(size*sizeof(struct map));
        mapping(root,res,code,code);

        out=fopen(fo,"w");
	fprintf(out,"%d\n",size);
        for(int i=1;i<=size;i++){
            fprintf(out,"%c->%s\n",res[i].c,res[i].code);
            printf("%c->%s\n",res[i].c,res[i].code);
        }

        printf("Generating %s\n",fo);
        inp=fopen(fi,"r");
        char* charCode[256];
        for(int i=1;i<=size;i++){
            charCode[(int)res[i].c] = strdup(res[i].code);
        }
            
        while(fscanf(inp,"%c",&read)!=EOF){
	    if (isspace(read)) {
	        fprintf(out,"%c", read);
		continue;
            }
            fprintf(out,"%s",charCode[(int)read]);
        }
        fclose(inp);
        fclose(out);
    }
    else if(mode==1){
        struct node* root1=NULL;
        char code[30];
        char* res1=malloc(sizeof(char));
        int length=0;
        int prevLen=0;
        char in[1024*1024];
	char* inPtr = in;
	int count = 0;
	int i = 0;
	fscanf(inp,"%d\n",&count);
	while (i < count) {
            fscanf(inp,"%c->%s\n",&read,code);
            prevLen = length;
            length+=strlen(code)+1;
            res1=realloc(res1,length*sizeof(char));
            sprintf(res1+prevLen, "%s%c", code, read);
	    i++;
        }
	fgets(in,1024*1024,inp);
        root1=mapdec(res1,length);

        printf("Generating %s\n",fo);
        out=fopen(fo,"w");
        while (inPtr[0] != '\0') {
          while (isspace(inPtr[0])) {
	    fprintf(out, "%c", inPtr[0]);
	    inPtr++;
	  }
	  if (inPtr[0] == '\0') break;
          inPtr = decoding(root1, inPtr, out);
        }
        fclose(out);
    }
    return 0;
}