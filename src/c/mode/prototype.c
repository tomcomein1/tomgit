//ԭ��ģʽ

typedef sturct _DATA{
	struct _DATA* (*copy)(struct _DATA* pData);
}DATA;

DATA data_A={data_copy_A};

struct _DATA* data_copy_A(struct _DATA* pData) {
	DATA* pResult = (DATA*)malloc(sizeof(DATA));
	assert(NULL != pResult);
	memmove(pResult, pData, sizeof(DATA) );
	return pResult;
};

struct _DATA* clone(struct _DATA* pData) {
	return pData->copy(pData);
}

