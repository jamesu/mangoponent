
typedef struct TestTypeStruct_s
{
    float x;
    float y;
} TestTypeStruct_t;


typedef void (*TestFuncFn)(Component_t*);
typedef void (*TestOtherFuncFn)(Component_t*,int);


typedef struct TestFuncTableStruct_s
{
    TestFuncFn testFunc;
    TestOtherFuncFn testOtherFunc;
} TestFuncTableStruct_t;
