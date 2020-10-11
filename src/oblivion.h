
/*
g++ -std=c++11  -I./include *.cpp -o c0c.exe
./c0c.exe

int g_list[20];
int testnum[10];

int abs(int a)
{
    g_list[0] = g_list[0] + 1;
    if (a < 0) {
        return (-a+ + 0);
    }
    return (a + -0);
}

void init()
{
    int i;
    i = 0;
    while (i < 10) {
        g_list[i] = 0;
        i = i + +1;
    }
    g_list[1] = g_list[1] + 1;
}

void show()
{
    int i;
    g_list[3] = 1;
    i = 0;
    while (i < 4) {
        printf(g_list[i]);
        i = i + 1;
    }
}

int times(int a, int b)
{
    g_list[2] = 1;
    return (a * b);
}

int sum4(int a, int b, int c, int d)
{
    g_list[3] = 1 + g_list[3];
    return (a + b + c + d);
}

void fi(int i)
{
    if (i) {
        testnum[i] = testnum[i+ + 1] + testnum[i- - 2];
        fi(i - 1);
    }
    else {
        testnum[i] = testnum[i+ + 1] + testnum[i- - 2];
        return;
    }
}


void main()
{
    int a;
    int theSum, i;
    char f;
    scanf(f, a);
    init();

    printf("abs(0) = ", abs(0));
    printf("abs(22) = ", abs(22));
    printf("abs(-1) = ", abs(-1));
    printf("abs(3) = ", abs(3));
    printf("abs(times(2, -2)) = ", abs(times(2, -2)));
    printf("sum4(4, 1, 3, 22) = ", sum4(4, 1, 3, 22));

    printf(sum4(abs(times(2, -2)), abs(-1), abs(-3), abs(22)));

    return;
}
int g_list[20];
int testnum[10];

int abs(int a)
{
    g_list[0] = g_list[0] + 1;
    if (a < 0) {
        return (-a+ + 0);
    }
    return (a + -0);
}

void init()
{
    int i;
    i = 0;
    while (i < 10) {
        g_list[i] = 0;
        i = i + +1;
    }
    g_list[1] = g_list[1] + 1;
}

void show()
{
    int i;
    g_list[3] = 1;
    i = 0;
    while (i < 4) {
        printf(g_list[i]);
        i = i + 1;
    }
}

int times(int a, int b)
{
    g_list[2] = 1;
    return (a * b);
}

int sum4(int a, int b, int c, int d)
{
    g_list[3] = 1 + g_list[3];
    return (a + b + c + d);
}

void fi(int i)
{
    if (i) {
        testnum[i] = testnum[i+ + 1] + testnum[i- - 2];
        fi(i - 1);
    }
    else {
        testnum[i] = testnum[i+ + 1] + testnum[i- - 2];
        return;
    }
}


void main()
{
    int a;
    int theSum, i;
    char f;
    scanf(f, a);
    init();

    printf(sum4(abs(times(2, -2)), abs(-1), abs(-3), abs(22)));

    testnum[9] = 1;
    testnum[7 + 'b' - 'a'] = 2;
    fi(7);
    printf("7 + 'b' - 'a' = ", 7 + 'b' - 'a');

    printf("testnum[2] = ", testnum[testnum[8]]);
    printf("testnum[1] = ", testnum[1]);
    printf("testnum[3] = ", testnum[testnum[testnum[5]] + testnum[9]]);
    printf("testnum[0] = ", testnum[0]);

    theSum = 0;
    if (a > 10) {
        i = 0;
        do {
            theSum = theSum + i;
            i = i + 1;
        } while (i < a);
    }
    else {
        if (f + 0 == '+' + 0) {
            i = 0;
            while (i < a + 3) {
                theSum = theSum + i;
                i = i + 3;
            };
        }
        else {
            i = 0;
            while (i < a + 3) {
                theSum = theSum - i;
                i = i + 3;
            };
        }
    }

    show();
    printf(theSum);

    return;
}
int testnum[10];

void fi(int i)
{
    if (i) {
        testnum[i] = testnum[i+ + 1] + testnum[i- - 2];
    }
    else {
        testnum[i] = testnum[i+ + 1] + testnum[i- - 2];
        return;
    }
}


void main()
{
    int a;
    int theSum, i;
    char f;
    scanf(f, a);

    testnum[9] = 1;
    testnum[7 + 'b' - 'a'] = 2;

    printf("testnum[2] = ", testnum[testnum[8]]);
    printf("testnum[1] = ", testnum[1]);
    printf("testnum[3] = ", testnum[testnum[testnum[5]] + testnum[9]]);
    printf("testnum[0] = ", testnum[0]);
    printf("testnum[7] = ", testnum[7]);
    printf("testnum[8] = ", testnum[8]);
    printf("testnum[9] = ", testnum[9]);

    fi(7);

    printf("testnum[2] = ", testnum[testnum[8]]);
    printf("testnum[1] = ", testnum[1]);
    printf("testnum[3] = ", testnum[testnum[testnum[5]] + testnum[9]]);
    printf("testnum[0] = ", testnum[0]);
    printf("testnum[7] = ", testnum[7]);
    printf("testnum[8] = ", testnum[8]);
    printf("testnum[9] = ", testnum[9]);

    return;
}
int testnum[10];

int abs(int a)
{
    if (a < 0) {
        return (-a+ + 0);
    }
    return (a + -0);
}

int times(int a, int b)
{
    return (a * b);
}

int sum4(int a, int b, int c, int d)
{
    return (a + b + c + d);
}


void main()
{
    int a;
    int theSum, i;
    char f;

    printf("times(2, -2) = ", times(2, -2));
    printf("abs(times(2, -2)) = ", abs(times(2, -2)));
    printf("abs(-1) = ", abs(-1));
    printf("abs(-3) = ", abs(-3));
    printf("abs(22) = ", abs(22));
    printf(sum4(abs(times(2, -2)), abs(-1), abs(-3), abs(22)));

    printf("7 + 'b' - 'a' = ", 7 + 'b' - 'a');

    return;
}

void main()
{
    int num1;
    int num2;
    int i, j, k;

    num2 = 1;
    num1 = 1;
    while (0 < num1)
    {
        num1 = num1 - 1;
        for (i = 0; i < 10; i = i + 1)
        {
            do
            {
                printf("Atta boy: ", num2);
                num2 = num2 - 1;
            } while (num2 > 0);
            for (j = 10; j > 0; j = j - 2)
            {
                printf("Atta boy %d%d!\n", 0 * 300 + (-0 - j) / 2 - 100);
            }
        }
    }

}

void main() {
    const int fina = +1, final = -2;
    const char ch = 'h';
    char identitych2;
    int mynum;
    int yournum;
    int i, j, k;

    scanf (yournum);
    mynum = 12;
    while (mynum > 0)
    {
        printf ("Atta boy 1!\n");
        mynum = mynum - 1;
    }

    do {
        printf ("Atta boy 2!\n");
        yournum = yournum - 1;
    } while (yournum > 0);

    for (i = 0; i < 10; i = i + 1)
    {
        printf ("Atta boy 3!\n");
    }
    for (j = 10; j > 0; j = j - 2)
    {
        printf ("Atta boy 3!\n");
    }
    for (k = 0; k != 0; k = k + 0)
    {
        printf ("Atta boy shouldn't be here!\n");
    }
    for (i = 0; i < 10; i = i + 1)
    for (j = 10; j > 0; j = j - 2)
    for (k = 0; k != 0; k = k + 0)
    while (mynum > 0)
    while (mynum <= 0)
    while (mynum != 0)
    do {
        yournum = yournum - 1;
    } while (yournum > 0);
    do {
        yournum = yournum - 1;
    } while (0);

}
int g_list[20];
int testnum[10];


void main()
{
    int a;
    int theSum, i;
    char f;
    i = 0;

    printf("testnum[2] = ", testnum[testnum[8]]);
    printf("testnum[1] = ", testnum[1]);
    printf("testnum[3] = ", testnum[testnum[testnum[5]] + testnum[9]]);
    printf("testnum[0] = ", testnum[0]);

    for (a = 1; a < 10; a = a + 1) {
        g_list[a] = g_list[a - 1] + 1;
        printf("glist[a] = ", g_list[a]);
    }
    printf("**********1***********");
    for (a = 1; (-a + 10) ; a = a + 1) {
        g_list[a] = g_list[a - 1] + 1;
        printf("glist[a] = ", g_list[a]);
    }
    printf("**********2 **********");
    for (a = 5; -a ; a = a - 1) {
        g_list[a] = g_list[a - 1] + 1;
        printf("glist[a] = ", g_list[a]);
    }
    printf("********** 3 ***********");
    for (a = -5 * 1 - 1 / 1 + +1 - 1 / 1; -a ; a = a + 1) {
        g_list[-a] = a;
        printf("glist[a] = ", g_list[-a]);
    }
    printf("********** 4 *********");
    for (a = a; a > 0; a = a - 1) {
        g_list[a] = g_list[a-+ 1] + 1;
        printf("glist[a] = ", g_list[a]);
    }
    printf("********** 5 ***********");
    for (a = -17; a < 0; a = a + 1) {
        g_list[-a] = g_list[-a-+ 1] + 1;
        printf("glist[a] = ", g_list[a]);
    }
    printf("*********************");

}
const int MAX_NUM = 128 ;

int factorial(int n){
   if(n<=1) return (1);

   return(n*factorial(n-1)) ;
}

int mod(int x, int y){
   x=x-x/y*y;

   return (x) ;
}

void swap(int x, int y){
    int temp;

    printf("x = ", x) ;
    printf("y = ", y) ;
    temp = x;
    x=y;
    y=temp;
    printf("SWAP x = ", x) ;
    printf("SWAP y = ", y) ;
}

int full_num(int n, int j, int a){
  return (n*100+j*10+a) ;
}

int flower_num(int n, int j, int a){
  return (n*n*n+j*j*j+a*a*a) ;
}

void complete_flower_num()
{
  int k[128];
  int i,j,n,s,x1,y;
  int m,k2,h,leap,x2;
  int a,b,c ;


  for(j=2;j< MAX_NUM ;j=j+1)
  {
    n = -1;
    s = j;
    for(i=1; i<j; i=i+1)
    {
      x1 = (j/i) * i ;
      if( mod(j,i) == 0 )
      {
        n = n + 1;
        s = s - i;
        if (n >= 128)
           printf("OVERFLOW!") ;
        else
           k[n] = i;
      }
    }

    if(s==0)
    {
      printf("complete number: ",j);
      for(i=0;i<=n;i=i+1)
        printf("  ",k[i]);
      printf(" ") ;
    }
  }

  printf("- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- -- -- -- -- -- -- --"); printf("'water flower'number is:"); y = 0 ;
  for(i=100;i<100+MAX_NUM;i=i+1){
      n=i/100;
      j=mod(i/10,10);
      a=mod(i,10);
      if(full_num(n,j,a)==flower_num(n, j, a)){
        k[y] = i ;
        y = y + 1 ;
      }
  }
  for(i = 0 ; i<y ; i=i+1){
    printf("  ", k[i]) ;
  }
  printf(" ") ;



  printf("- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- -- -- -- -- -- -- --"); h = 0 ; leap = 1 ; for(m = 2 ; m <= MAX_NUM ; m=m+1)
  {
    k2 = m / 2;
    for(i=2; i<=k2; i=i+1){
      x2 = (m/i)*i ;
      if( mod(m,i) == 0)
      {
        leap=0;
      }
     }
    if(leap == 1)
    {
      printf(" ",m);
      h = h +1;

      x2 = (h/10)*10 ;
      if( x2 == h)
        printf(" ");
     }
     leap=1;
  }

  printf("The total is ",h);

}



void main()
{
   int n ;

   n = factorial(5) ;
   printf("5 != ", n) ;

   swap(5, 10 ) ;

   complete_flower_num() ;
}
const int MAX_NUM = 10;


void main()
{
    int i, j, n;
    int k[48];

    n = 0;
    for (j = 2; j < MAX_NUM; j = j + 1) {
        for (i = 1; i < j; i = i + 1) {
            printf("***********************");
            printf("i = ", i);
            printf("j = ", j);

            if (i <= 1) {
                n = n + 1;
                if (n < 48)
                    k[n] = i;
            }
        }
    }
}
const char ch = 'd';
char arr[10];

char func(char ch)
{
    return (ch);
}

void main()
{
    printf(ch);
    printf(func(ch));
    arr[1] = 'c';
    printf(arr[1]);

    printf("'?' = ", '?');
    printf("ch = ", ch);
    printf("func(ch) = ", func(ch));
    printf("arr[1] = ", arr[1]);
    return ;
}
int g_list[20];
int testnum[10];

void init()
{
    int i;
    i = 0;
    while (i < 10) {
        g_list[i] = 0;
        i = i + +1;
    }
    g_list[1] = g_list[1] + 1;
}


void show()
{
    int i;
    g_list[3] = 1;
    i = 0;
    while (i < 4) {
        printf(g_list[i]);
        i = i + 1;
    }
}


void main()
{
    int a;
    int theSum, i;
    char f;
    init();

    show();

    return;
}int g_list[20];
int testnum[10];

void main()
{
    int a, b, c, d, e, g, h;
    int theSum, i;
    char f;
    i = 0;

    printf("res = ", 1 + 3 * 2 / 4 - 5);
    a = 1 + 3 * 2 / 4 - 5;
    b = -2;
    c = 1 --2;
    d = -1 --2;
    printf(a);
    printf(b);
    printf(c);
    printf(d);
    a = -1 + 2;
    b = +1 --2 * a;
    c = +1 - +2;
    d = +1 - 2 + c;
    printf("res = ", i);
    printf(a);
    printf(b);
    printf(c);
    printf(d);
}
int g_list[20];
int testnum[10];

int abs(int a)
{
    g_list[0] = g_list[0] + 1;
    if (a < 0) {
        return (-a+ + 0);
    }
    return (a + -0);
}

int times(int a, int b)
{
    g_list[2] = 1;
    return (a * b);
}

void main()
{
    int a;

    printf(abs(times(2, -2)));
}
void main(){

   int a,c;

   char b,d;



   scanf(a,b);
   scanf(c,d);



   printf("a: ",a);

   printf("b: ",b);

   printf("c: ",c);

   printf("d: ",d);



   printf("a+1: ",a+1);

   printf("b+1: ",b+1);

   printf("c+1: ",c+1);

   printf("d+1: ",d+1);



   printf("a+c: ",a+c);

   printf("b+d: ",b+d);



}
const int MAX_NUM = 128 ;

int factorial(int n){
   if(n<=1) return (1);

   return(n*factorial(n-1)) ;
}

int mod(int x, int y){
   x=x-x/y*y;

   return (x) ;
}

void swap(int x, int y){
    int temp;

    printf("x = ", x) ;
    printf("y = ", y) ;
    temp = x;
    x=y;
    y=temp;
    printf("SWAP x = ", x) ;
    printf("SWAP y = ", y) ;
}

int full_num(int n, int j, int a){
  return (n*100+j*10+a) ;
}

int flower_num(int n, int j, int a){
  return (n*n*n+j*j*j+a*a*a) ;
}

void complete_flower_num()
{
  int k[128];
  int i,j,n,s,x1,y;
  int m,k2,h,leap,x2;
  int a,b,c ;


  for(j=2;j< MAX_NUM ;j=j+1)
  {
    n = -1;
    s = j;
    for(i=1; i<j; i=i+1)
    {
      x1 = (j/i) * i ;
      if( mod(j,i) == 0 )
      {
        n = n + 1;
        s = s - i;
        if (n >= 128)
           printf("OVERFLOW!") ;
        else
           k[n] = i;
      }
    }

    if(s==0)
    {
      printf("complete number: ",j);
      for(i=0;i<=n;i=i+1)
        printf("  ",k[i]);
      printf(" ") ;
    }
  }

  printf("- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- -- -- -- -- -- -- --"); printf("'water flower'number is:"); y = 0 ;
  for(i=100;i<100+MAX_NUM;i=i+1){
      n=i/100;
      j=mod(i/10,10);
      a=mod(i,10);
      if(full_num(n,j,a)==flower_num(n, j, a)){
        k[y] = i ;
        y = y + 1 ;
      }
  }
  for(i = 0 ; i<y ; i=i+1){
    printf("  ", k[i]) ;
  }
  printf(" ") ;



  printf("- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- -- --
-- -- -- -- -- -- -- --"); h = 0 ; leap = 1 ; for(m = 2 ; m <= MAX_NUM ; m=m+1)
  {
    k2 = m / 2;
    for(i=2; i<=k2; i=i+1){
      x2 = (m/i)*i ;
      if( mod(m,i) == 0)
      {
        leap=0;
      }
     }
    if(leap == 1)
    {
      printf(" ",m);
      h = h +1;

      x2 = (h/10)*10 ;
      if( x2 == h)
        printf(" ");
     }
     leap=1;
  }

  printf("The total is ",h);

}



void main()
{
   int n ;

   n = factorial(5) ;
   printf("5 != ", n) ;

   swap(5, 10 ) ;

   complete_flower_num() ;
}
const int glb_const = 157;
int test;

void main()
{
    const int main_const_1 = -188, main_const_2 = 0;
    char a, b;

    printf("Hello World");
    scanf(test);
    scanf(a, b);
    test = b * 2 + a - 6 / 2;
    printf("wdnmd", 1 * test + 2 / 3 - 1 * glb_const);
}
const int glb_const_int_1 = 0;
const int glb_const_int_2 = -2147364830;
const int glb_const_int_3 = +2, glb_const_int_4 = -3333;
const int glb_const_int_5 = 111, glb_const_int_6 = -1, glb_const_int_7 = 0;

const char glb_const_char_1 = '0';
const char glb_const_char_2 = '_';
const char glb_const_char_3 = 'a', glb_const_char_4 = 'z';
const char glb_const_char_5 = '+', glb_const_char_6 = '-', glb_const_char_7 =
'/', glb_const_char_8 = '*';

int glb_int_1;
int glb_int_2;
int glb_int_3, glb_int_4;
int glb_int_5, glb_int_6, glb_int_7;
int glb_int_8[2], glb_int_9, glb_int_10[32], glb_int_11[1];

char glb_char_1;
char glb_char_2;
char glb_char_3, glb_char_4;
char glb_char_5, glb_char_6, glb_char_7;
char glb_char_8[2], glb_char_9, glb_char_10[32], glb_char_11[1];

int int1Function1(int tiny)
{
    const int func_const_int_1 = 0;
    const char func_const_char_1 = '0';
    int func_int_1;
    char func_char_1, func_char_2;

    return (tiny + 1);
}

int int2Function1(int a, int b)
{
    const int func_const_int_1 = 0;
    const char func_const_char_1 = '0';

    return (a + int1Function1(b));
}

int int0Function1()
{
    char f1ch1;
    int f1int1;

    f1int1 = -512;
    f1int1 = f1int1 + 3 * 4 / 5 - 2;
    if (1 >= f1int1)
    {
        f1int1 = f1int1 + 100;
    }
    else if (f1int1 <= 3)
    {
        f1int1 = f1int1 * 2;
    }
    else if (f1int1 == 166)
    {
        f1int1 = f1int1 - 2;
    }
    else
    {
        f1int1 = f1int1 / 10;
    }

    if (f1int1 > 9999)
    {
        return (3);
    }
    else
    {
        if (f1int1 != 32)
        {
            f1int1 = 3 * 4 - 5 * int1Function1(f1int1) + 2;
        }
        return (f1int1);
    }

    return (f1int1);
}
char char0Function1()
{
    return ('c');
}
char char1Function1(char ch)
{
    char constch;

    constch = '_';
    return (constch);
}

char char2Function1(int a, char ch)
{
    return (char1Function1(ch));
}
void v0Function1()
{
    const char ch = 'd';

    return;
}
void v1Function1(char ch)
{
    return;
}

void v2Function1(int a, char ch, int b)
{
    return;
}


void main()
{
    const int main_const_int_1 = 0;
    const int main_const_int_2 = -2147364830;
    const int main_const_int_3 = +2, main_const_int_4 = -3333;
    const int main_const_int_5 = 111, main_const_int_6 = -1, main_const_int_7 =
0;

    const char main_const_char_1 = '0';
    const char main_const_char_2 = '_';
    const char main_const_char_3 = 'a', main_const_char_4 = 'z';
    const char main_const_char_5 = '+', main_const_char_6 = '-',
main_const_char_7 = '/', main_const_char_8 = '*';

    int main_int_1;
    int main_int_2;
    int main_int_3, main_int_4;
    int main_int_5, main_int_6, main_int_7;
    int main_int_8[2], main_int_9, main_int_10[32], main_int_11[1];

    char main_char_1;
    char main_char_2;
    char main_char_3, main_char_4;
    char main_char_5, main_char_6, main_char_7;
    char main_char_8[2], main_char_9, main_char_10[32], main_char_11[1];

    int num1;
    int num2;
    int i, j, k;
    char ch1;

    v0Function1();

    main_char_1 = '_';
    num2 = +num1;
    num2 = +main_int_8[0];
    num2 = +(+(-(0)));
    num2 = +888;
    ch1 = +'-';
    num2 = +int0Function1();
    num2 = +main_const_int_1;

    num2 = -num1;
    num2 = -main_int_8[0];
    num2 = -(+(-(0)));
    num2 = -888;
    num2 = -int0Function1();
    num2 = -main_const_int_1;


    num2 = num1;
    num2 = main_int_8[0];
    num2 = (+(-(0)));
    num2 = 888;
    ch1 = '-';
    num2 = int0Function1();
    num2 = main_const_int_1;

    glb_int_8[0] = 111;

    num1 = 1;
    while (0 < num1)
    {
        num1 = num1 - 1;
        printf(1 + num1);
        printf(char1Function1(main_char_1));
        printf(char2Function1(j, main_char_1));
        num1 = num1 - 1;
        for (i = 0; i < 10; i = i + 1)
        {
            do
            {
                printf("Atta boy: ", num2);
                num2 = num2 - 1;
            } while (num2 > 0);
            for (j = 10; j > 0; j = j - 2)
            {
                printf("Atta boy %d%d!\n", 0 * 300 + (int0Function1() - j) / 2 -
100);
            }
        }
    }

}
const int glb_const = 157;
const int glb_const_1 = -17, glb_const_2 = +18;
int test;
int glb_int_1, glb_int_2;

void main()
{
    int a, b, c, d;
    char ch, ch2;
    a = -1;
    scanf(a, b, c);
    scanf(ch);
    scanf(d, ch2);
    printf("a /= a");
    printf(a);
    printf(a / a);
    printf(a / a / a);
    printf(a / a / a / a);
}
char ch;
void main()
{
    int a;
    printf("2 * (1 + 3): ", 2 * (1 + 3));
    printf("2 * (1 + 3) - 1: ", 2 * (1 + 3) - 1);
    printf("2 * (1 + 3) / 2 - 1: ", 2 * (1 + 3) / 2 - 1);
}
char ch;
void main()
{
    int a;
    a = -3;
    printf("hello\n");
    printf("\n", a);
    printf("a = \n", a);
}
int g_list[20];

void init(int i)
{
    if (i > 0) {
        g_list[i] = g_list[i] + 1;
    }
    else {
        i = 1;
    }
}


void main()
{
    int theSum[2], i;
    scanf(i);
    init(i);
    printf("i: ", i);
    printf("glist[i] = ", g_list[i]);
}
int testnum[10];


void main()
{
    testnum[0] = 'b' - 'a';

    testnum[5] = 8;
    testnum[9] = 1;
    testnum[7 + 'b' - 'a'] = 2;
    printf("7 + 'b' - 'a' = ", 7 + 'b' - 'a');

    printf("testnum[8] = ", testnum[8]);
    printf("testnum[testnum[8]] = ", testnum[testnum[8]]);
    printf("testnum[2] = ", testnum[testnum[8]]);

    printf("testnum[1] = ", testnum[1]);

    printf("testnum[5] + testnum[9] = ", testnum[5] + testnum[9]);
    printf("testnum[testnum[5]] + testnum[9] = ", testnum[testnum[5]] +
testnum[9]); printf("testnum[testnum[testnum[5]] + testnum[9]] = ",
testnum[testnum[testnum[5]] + testnum[9]]);

    printf("testnum[3] = ", testnum[testnum[testnum[5]] + testnum[9]]);
    printf("testnum[0] = ", testnum[0]);
}
int testnum[10];


void main()
{
    testnum[0] = 'b' - 'a';

    testnum[5] = 8;
    testnum[9] = 1;
    testnum[7 + 'b' - 'a'] = 2;
    printf("7 + 'b' - 'a' = ", 7 + 'b' - 'a');

    printf("testnum[8] = ", testnum[8]);
    printf("testnum[testnum[8]] = ", testnum[testnum[8]]);
    printf("testnum[2] = ", testnum[testnum[8]]);

    printf("testnum[1] = ", testnum[1]);

    printf("testnum[5] + testnum[9] = ", testnum[5] + testnum[9]);
    printf("testnum[testnum[5]] + testnum[9] = ", testnum[testnum[5]] +
testnum[9]); printf("testnum[testnum[testnum[5]] + testnum[9]] = ",
testnum[testnum[testnum[5]] + testnum[9]]);

    printf("testnum[3] = ", testnum[testnum[testnum[5]] + testnum[9]]);
    printf("testnum[0] = ", testnum[0]);
}

*/