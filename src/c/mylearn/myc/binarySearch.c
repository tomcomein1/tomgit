/*二分查找，用位方式*/
int binarySearch(int a[], int target)
{
	int low = 0;
	int high = lengthA(a) -1 ;
	int mid, midVal;

	while (low <= high) 
	{
		mid = calculateMidpoint(low + high);
		midVal = a[mid];
		if (midVal < target)
			low = mid + 1;
		else if (midVal > target)
			high = mid - 1;
		else return mid;	
	}
	return -1;
}

int calculateMidpoint(int low, int high)
{
    return (low + high) >> 1;
}

int lengthA(int a[])
{
	int len=0;
	for (; a[len] != '\0'; len++) ;
	return len;
}

main ()
{
	int a[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, -1};
	int unit = binarySearch(a, 6);
	
	printf ("found: a[%d]: %d\n", unit, a[unit]);
}

