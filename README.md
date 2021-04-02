# Kalzn_Code_Judger
这是一个自己做着玩的玩具，很简单，完成了代码评测机基本功能。
#### 就目前而言，本程序可以：

1、对一个可执行文件进行评测，并监控判定其所使用的时间、空间资源。

2、在进程结束运行后得出其是否是正常退出（判定有没有出现数字越界、爆栈、除0等会导致runtime error的情况。）

3、对已经超时的进程进行截断。

4、对输出结果和标准答案进行匹配，得出可执行文件的输出结果以及格式是否正确。

5、对进程所耗资源与约定资源进行比较，得出待测程序是否超时/内存。

6、支持严格和非严格两种评测方式。

#### 本程序计划，但没有实现的功能：

1、对程序耗栈大小进行监控。

2、接入使用SPJ程序对待测程序输出文件进行测试（计划使用codeforces的Testlib 库）

3、对内存超限的进程进行截断。（目前没有找到什么比较好的方法监控内存。）

4、Output limit exceeded的判定没有实现。

5、对评测信息，尤其事system error进行log（懒）。

目前只在centOS7测试过，理论适用于所有Linux。

#### 试用方法
在目录下
```sh
g++ -std=c++11 Judger.h Judger.cpp -o Judger
g++ -std=c++11 Runner.h Runner.cpp -o runner
g++ -std=c++11 Killer.h Killer.cpp -o killer
```
保证生成的可执行文件Judger、 runner、 killer在同一目录下。
输入
```sh
./Judger <可执行文件路径> <输入数据文件路径> <输出数据文件路径>
```
即可完成测试，评测结果在同目录下的1.txt。


#### 正式使用

可以调用Judger中的run函数开始测评。
而runner， killer是两个辅助进程。runner用于执行待评测程序，killer是超时截断。
调用run函数是需指明两个辅助可执行文件的路径。


调用函数：
```cpp
 static void Judger::run(JudgeConfig task, RunConfig* result, const string & RunnerExePath, const string & KillerExePath);
```
后两个参数即为上述所说runner、 killer的辅助程序路径。

第一个参数为JudgeConfig，传入本次评测信息：
```cpp
class JudgeConfig {

public:
    int MaxCPUTime; //最大CPU时限
    long long MaxMemory;  //最大内存使用量
    long long MaxStack; //最大栈
    long long MaxOutputSize; //最大输出大小
    short CompilerCode; // 编译器代码


    string ExePath; // 带评测程序路径
    string InputDataPath; //输入文件路径
    string OutputDataPath; //输出文件路径
    string StdAnswerPath; //标准答案路径
    string LogPath; //log路径
    bool isSPJ; //是否使用SPJ
    string SPJExePath;//SPJ路径

    JudgeConfig();
    
};
```
第二个参数为RunConfig，存入评测结果：
```cpp
class RunConfig {
public:
    int UseCPUTime; //CPU时间消耗
    long long UseMemory;//内存消耗
    long long UseMaxStack;//栈消耗
    long long UseOutputSize;//输出大小
    int ExitSignal;//结束信号量
    int ExitCode;//退出代码
    int Error;//错误代码


    int Result;//结果
    string ResultDetail;//结果解释
};
```
还可以设置其他的评测参数
```cpp
class Judger {
private:
    static JudgeConfig config;
    const static int MAXBUF = 1e6; //数据字符最大缓存
    const static int MAX_OVERFLOW_MULTIPLE = 2; //输出最大倍数
    /*
    在评测时，首先读入标准答案，然后读入待评测程序的输出，如果待评测程序的输出大小大于标准答案的MAX_OVERFLOW_MULTIPLE倍，
    即返回结果OUTPUT_LIMIT_EXCEEDED 输出超限
    */


    const static bool StrictMode = true;
    /*
    严格评测模式：
    如果为true，则启用严格评测模式，在严格模式下，按照以下顺序比对答案和输出：
    1、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
    2、如果输出和答案相等，则AC。
    3、否则：将答案和输出的所有连续空白字符都用一个空格替代。
    4、如果此时输出和答案相等，则PE。
    5、否则WA。

    在非严格模式下：
    1、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
    3、将答案和输出的所有连续空白字符都用一个空格替代。
    4、如果此时输出和答案相等，则AC。
    5、否则WA。

    */
    static string ansbuf, outbuf;
    static RunConfig CreateSystemError(string me);
    static void ExecKiller(string KillerExePath, pid_t RunnerId, int KillTime);
    static void ExecRunner(string RunnerExePath, string ExePath, string InputDataPath, string OutputDataPath);
    static void GetResult(RunConfig* result, int status, rusage resource_usage);
    static void CheckAnswer( RunConfig* result);
    static void FormatString (string &s);
public:
    static void run(JudgeConfig task, RunConfig* result, const string & RunnerExePath, const string & KillerExePath);
};

```
返回的结果有

```cpp
class JudgerResult{
public:
    const static int ACCEPTED = 0;  // 答案正确
    const static int WRONG_ANSWER = 1; // 答案错误
    const static int TIME_LIMIT_EXCEEDED = 2; //时间超限(CPU时间超限)
    const static int MEMORY_LIMIT_EXCEEDED = 3; //内存超限
    const static int RUNTIME_ERROR = 4; // 运行时错误
    const static int PRESENTATION_ERROR = 5;  // 格式错误(只在严格评测模式)
    const static int OUTPUT_LIMIT_EXCEEDED = 6;  //输出超限
    const static int SYSTEM_ERROR = 7; //系统错误(错误信息查看RunConfig中的ResultDetail)
};
```
## 只是玩具!!!

# Eng (chinglish) ver

This is just a toy made by me. It is very simple and implement the base function of code test.

#### what the program can do so far:
1) Test a executable file, monitor the cpu time and memory  resource consumeption.
2) Judge whether a program exit normally (Judge whether a program have a circumstance that leads to "runtime error" such as array access exception, stack exception, or divided by 0 and so on.)
3) Kill the timeout process.
4) Match the output and the standard answer, justify the result and format.
5) Compare the resource consumed and limited, test the program whether exceed the limitation of time or memory.
6) Two judge mode--strict and not strict is supplied.

#### what the program plan to do but not been implemented so far:
1) Monitor stack consumption.
2) Connect a SPJ program to test the file.
3) Kill memory limitation exceed process (have no idea to monitor the memory consumption during process is running).
4) The judgement of "Output limit exceeded" is not implemented.
5) Log information of judgement particular the system error.

Just debug in centOS7, suit all Liunx release version theoretically.

#### start try to use
Enter in the directory
```sh
g++ -std=c++11 Judger.h Judger.cpp -o Judger
g++ -std=c++11 Runner.h Runner.cpp -o runner
g++ -std=c++11 Killer.h Killer.cpp -o killer
```
Wait for updating.
