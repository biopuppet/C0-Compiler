const int const1 = 1, const2 = -100;
const char const3 = '_';
int change1;
char change3;

int gets1(int var1, int var2){
    change1 = var1 + var2;
    return (change1);
}

void main()
{
    printf("Hello World");
    printf(gets1(10, 20));
}