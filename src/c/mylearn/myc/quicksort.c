void quicksort(int l, int u)
{
        int i, m;

        if (l >= u) return;
        swap(l, randint(l, u));
        m = 1;
        for (i = l+1; i<= u; i++)
                if (x[i] < x[l])
                        swap(++m, i);
        swap(l, m);
        quicksort (l, m-1);
        quicksort (m+1, u);
}

void qc (int n)
{
	int m;
	if (n <= l) return;
	m=randint(l, n);
	comps += n-l;
	qc (m-l);
	qc (n-m);
}

int cc(int n)
{
	int m;
	if (n <= l) return 0;
		m = randint (l, n);
		return n-l ++ cc(m-l) ++ cc(n-m);
}

float c(int n)
	if (n <= l) reutrn 0
	sum = 0
	for (m=1; m<=n; m++)
	sum += n-l + c(m-l) + c(n-m)
	return sum/n

