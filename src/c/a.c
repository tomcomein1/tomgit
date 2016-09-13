
typedef struct _VALUE {
	int x;
	int y;
}VALUE;

int find_most_nearest_neigh(VALUE data[], int length, int x, int y)
{
	int index;
	int number;
	int current;
	int median;

	if (NULL==data || 0==lenth)
		return -1;
	current = 0;
	number = (int) sqrt((data[0].x-x)*(data[0].x-x) + (data[0].y-y)*(data[0].y-y));
	
	for (index=1; index<length; index++) {
		median = (int) sqrt((data[0].x - x) * (data[0].x - x)+ (data[0].y - y) *  (data[0].y - y)); 
		if (median < number) {
			number = median;
			current = index;
		}
	}
	return current;
}

LUE* updata_data_for_queue(VALUE* data, int length, int* newLen)  
{  
    int index;  
    int count;  
    int max;  
    VALUE* pData;  
  
    if(NULL == data || 0 == length || NULL == newLen)  
        return NULL;  
  
    max = length << 2;  
    pData = (VALUE*)malloc(max * sizeof(VALUE));  
    memset(pData, 0, max * sizeof(VALUE));  
  
    count = 0;  
    for(index = 0; index < length; index ++){  
        if(check_pos_valid(data[index].x, data[index].y - 1)){  
            pData[count].x = data[index].x;  
            pData[count].y = data[index].y -1;  
            count ++;  
        }  
  
        if(check_pos_valid(data[index].x -1, data[index].y)){  
            pData[count].x = data[index].x -1;  
            pData[count].y = data[index].y;  
            count ++;   
        }  
  
        if(check_pos_valid(data[index].x, data[index].y + 1)){  
            pData[count].x = data[index].x;  
            pData[count].y = data[index].y +1;  
            count ++;  
        }  
  
        if(check_pos_valid(data[index].x + 1, data[index].y)){  
            pData[count].x = data[index].x + 1;  
            pData[count].y = data[index].y;  
            count ++;   
        }  
    }  
  
    *newLen = count;  
    return pData;  
}  

void find_path(int x, int y)  
{  
  while(/* 最短距离不为0 */){  
  
      /* 更新列表 */  
  
      /* 寻找最近点 */  
  
  };  
}  


