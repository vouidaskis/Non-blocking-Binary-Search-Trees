#include<limits.h>
//#include<stdatomic.h>

struct info{

	struct node *info_node[3];
	struct info *pinfo;
};

struct search_result{

	struct node *parent, *grandparent, *leaf;
	struct info *pinfo, *gpinfo;
};

struct node 
{ 

	int key;
	struct node *left, *right; 
	struct info *info;

}; 

	//create search

struct search_result* search(struct node *root,int key){

	struct search_result *search_result =  (struct search_result *)malloc(sizeof(struct search_result));
	struct node *parent = NULL;
	struct node *grandparent = NULL;
	struct node *leaf = root;
	struct info *info = NULL;
	struct info *gpinfo = NULL;
	parent = leaf;
	info = parent->info;
	if ( key < parent->key ){// if i have a node then the grandparent is null 
		leaf = parent->left;
	}else{
		leaf = parent->right;
	}
	while (leaf->left != NULL ){
		grandparent = parent;
		parent = leaf;
		info = parent->info;
		gpinfo = grandparent->info;
		if ( key < parent->key ){
			leaf = parent->left;
		}else{
			leaf = parent->right;
		}

	}


	
	search_result->parent = parent;
	search_result->grandparent = grandparent;
	search_result->leaf = leaf;
	search_result->pinfo = info;
	search_result->gpinfo = gpinfo;
	return search_result;

}


void cas_child(struct node *parent, struct node *old, struct node *new){

	if( new->key < parent->key ){
	
		__sync_bool_compare_and_swap( &(parent->left), old, new);
	}else{

		__sync_bool_compare_and_swap( &(parent->right), old, new);
	}

}

struct info *fuct_clean(){

	struct info *info = (struct info *)malloc(sizeof(struct info));
	info->info_node[0] = NULL;
	info->info_node[1] = NULL;
	info->info_node[2] = NULL;
	info->pinfo = NULL;

	return info;
}

void help_mark(struct info *dinfo){

	struct node *other = NULL;
	struct node *parent = dinfo->info_node[1];
	struct node *leaf = dinfo->info_node[2];
	struct node *grandparent = dinfo->info_node[0];

	if( parent->right == leaf){

		other = parent->left;
		
	}else{

		other = parent->right;
		
	}

	cas_child(grandparent, parent, other);
//doing the state grandparent clean
	__sync_bool_compare_and_swap( &(dinfo->info_node[0]->info), dinfo, fuct_clean());

}

void help_insert(struct info *info){
	struct node *parent = info->info_node[1];
	struct node *leaf = info->info_node[2];
	struct node *newinternal = info->info_node[0];
		cas_child(parent, leaf, newinternal);
//doing the state parent clean
		__sync_bool_compare_and_swap( &(info->info_node[1]->info), info, fuct_clean()); 

}

int is_iflag(struct info *info, struct node *node){

	if(info == NULL && node !=NULL)return 1;
	return 0;
}

int is_mark(struct info *info, struct node *node){

	if(info != NULL && info->info_node[1] == node)return 1;
	return 0;
}

int is_dflag(struct info *info, struct node *node){

	if(info != NULL && info->info_node[0] == node)return 1;
	return 0;
}

void help(struct info *help_info, struct node *node){
	//check the state if is clean, flag, mark, dflag
	if(is_iflag(help_info->pinfo, help_info->info_node[1])  ){ // check if the state is iflag

		help_insert(help_info);
	}else if(is_mark(help_info->pinfo, node)){ // check if the state is mark

		help_mark(help_info);
	}else if(is_dflag(help_info->pinfo, node)){ // check if the state is dflag

		help_delete(help_info, node);
	}
}

struct node *new_internal(struct node *node, int key){

	struct info *new_info = (struct info *)malloc(sizeof(struct info));//create new info for new leaf
	struct info *info = (struct info *)malloc(sizeof(struct info));// creat new info for new internal
	struct node *new_sibling = (struct node *)malloc(sizeof(struct node)); // creat new leaf
	struct node *newinternal = (struct node *)malloc(sizeof(struct node)); //creat new internal
	info->info_node[0] = NULL;
	info->info_node[1] = NULL;
	info->info_node[2] = NULL;	
	info->pinfo = NULL;
	new_info->info_node[0] = NULL;
	new_info->info_node[1] = NULL;
	new_info->info_node[2] = NULL;	
	new_info->pinfo = NULL;
	new_sibling->key = key;
	new_sibling->left = NULL;
	new_sibling->right = NULL;
	new_sibling->info = new_info;

		if( key < node->key ){// I check whether the new key is less than the key of the existing leaf

			newinternal->left = new_sibling;
			newinternal->right = node;
			newinternal->key = node->key;
			newinternal->info = info;
		}else{

			newinternal->left = node;
			newinternal->right = new_sibling;
			newinternal->key = key;
			newinternal->info = info;
		}

	return newinternal;
}

int insert(struct node *root, int key){
	
	struct info *info = (struct info *)malloc(sizeof(struct info)); // create new info
	struct search_result *search_result = (struct search_result *)malloc(sizeof(struct search_result));
	
	info->info_node[0] = NULL;
	info->info_node[1] = NULL;
	info->info_node[2] = NULL;	
	info->pinfo = NULL;
	
	while(1){
		//doing search the tree for new key
		search_result = search(root, key);
		//check if exist the key
		if(search_result->leaf->key == key){

			return 0;
		}//check if parent is clean
		if( search_result->pinfo->info_node[1] != NULL && search_result->parent != NULL ){

			help(search_result->pinfo, search_result->parent);
		}else{
			info->info_node[0] = new_internal(search_result->leaf, key); // creating new internal
			info->info_node[1] = search_result->parent;// i put in info the parent
			info->info_node[2] = search_result->leaf; //i put info leaf  the existing leaf the tree
			info->pinfo = NULL;
			// check if the info parent has change
			if( __sync_bool_compare_and_swap( &(search_result->parent->info), search_result->pinfo, info)){

				help_insert(info);

				return 1;
			}else{ 

				help(search_result->parent->info,search_result->parent);
			}
		
		}

	}
	
}

int help_delete(struct info *dinfo, struct node *parent){
	
	struct info *result;
	result = __sync_val_compare_and_swap( &(dinfo->info_node[1]->info), dinfo->pinfo, dinfo);
 
	//check if compare and swap is correct in other operation has change this info
	if( result == dinfo->pinfo || dinfo->info_node[1]->info == dinfo){

		help_mark(dinfo);		

		return 1;
	}else{

		help(dinfo->info_node[1]->info, dinfo->info_node[1]);
		__sync_bool_compare_and_swap( &(dinfo->info_node[0]->info), dinfo, fuct_clean());
		return 0;
	}

}

int delete(struct node *root, int key){
	struct search_result *search_result = (struct search_result *)malloc(sizeof(struct search_result));
	struct info *info = (struct info *)malloc(sizeof(struct info));//create new info
	info->info_node[0] = NULL;
	info->info_node[1] = NULL;
	info->info_node[2] = NULL;	
	info->pinfo = NULL;
	
	while(1){

		//doing search in tree for new key
		search_result=search(root,key);
		//check if exist the new key
		if( search_result->leaf->key != key ){ 

			 return 0;
		}//check if the state grandparent is clean
		 if(search_result->gpinfo->info_node[0] != NULL ){

			help(search_result->gpinfo, search_result->grandparent);
		}else if(search_result->pinfo->info_node[1] != NULL ){//check if the state parent is clean

			help(search_result->pinfo, search_result->parent);//search_search->pinfo
		}else{
			info->info_node[0] = search_result->grandparent;// i put dinfo the grandparent
			info->info_node[1] = search_result->parent;// i put dinfo the parent
			info->info_node[2] = search_result->leaf;//i put dinfo the leaf
			info->pinfo =  search_result->pinfo;// i put dinfo the update for parend this help for help_delete
			//check if info grandparent has change
			if( __sync_bool_compare_and_swap( &(search_result->grandparent->info), search_result->gpinfo, info)){
				
				if(help_delete(info, search_result->parent))return 1;
			}else{

				help(search_result->grandparent->info, search_result->grandparent);			
			}
		}
	}
}

int find(struct node *root, int key){
	struct search_result *search_result = (struct search_result *)malloc(sizeof(struct search_result));

	search_result=search(root,key);

	if(search_result->leaf->key == key ){

		 return 1;

	}else return 0;
		
}

//========================= A utility function to do inorder BST =========================

static int total_leaf = 0;
static long total_key_tree = 0;

void total(){ // delete
	total_leaf=0;
	total_key_tree = 0;    
}
	 
long inorder_return_total(struct node *root){
 
	if (root != NULL) {

		inorder_return_total(root->left); 
		if(root->left == NULL){

		total_leaf = total_leaf + 1; 

		}
		inorder_return_total(root->right); 
	} 

	return total_leaf;
}

long inorder_return_sum(struct node *root){//node
 
	if (root != NULL) {
//printf("%ld\n",root);
		inorder_return_sum(root->left); 
		if(root->left == NULL){
		
		total_key_tree = total_key_tree + root->key;

		}
		inorder_return_sum(root->right); 
	} 

	return total_key_tree;
} 


