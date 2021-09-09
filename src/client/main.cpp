

template<typename T>
class base {
	T x;
};

class derive : public base<int> {
	int y;
};

int fun(base<int>* b) {
	return 1;
}

int main() 
{
	derive* d = new derive();
	int x = fun(d);
	return 0;
}