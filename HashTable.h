//the table itself has a sentinel, number of keys(for convenience)
//and a bucket pointer array
typedef struct
{
	int sentinel;
	int keys;
	struct entry **buckets;

} hashTable;

typedef hashTable* HashTablePTR;

int CreateHashTable( HashTablePTR *handle, unsigned int initialSize );

int DestroyHashTable( HashTablePTR *handle );

int InsertEntry( HashTablePTR hashTable, char *key, void *data, void **existingDataHandle );

int DeleteEntry( HashTablePTR hashTable, char *key, void **data );

int FindEntry( HashTablePTR hashTable, char *key, void **data );

int GetKeys( HashTablePTR hashTable, char ***keys, unsigned int *keyCount );

int GetLoadFactor( HashTablePTR hashTable, float *loadFactor );
