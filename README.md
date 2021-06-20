# Kalzn_Code_Judger
这是一个自己做着玩的玩具，很简单，完成了代码评测机基本功能。
#### 就目前而言，本程序可以：

1、对一个可执行文件进行评测，并监控判定其所使用的时间、空间资源。

2、在进程结束运行后得出其是否是正常退出（判定有没有出现数字越界、爆栈、除0等会导致runtime error的情况。）

3、对已经超时的进程进行截断。

4、对输出结果和标准答案进行匹配，得出可执行文件的输出结果以及格式是否正确。

5、对进程所耗资源与约定资源进行比较，得出待测程序是否超时/内存。

6、支持严格和非严格两种评测方式。

7、使用SPJ进行评测。

#### 本程序计划，但没有实现的功能：

1、对程序耗栈大小进行监控。

2、接入使用SPJ程序对待测程序输出文件进行测试（计划使用codeforces的Testlib 库）

3、对内存超限的进程进行截断。（目前没有找到什么比较好的方法监控内存。）

4、Output limit exceeded的判定没有实现。

5、对评测信息，尤其事system error进行log（懒）。

目前只在centOS7测试过，理论适用于所有Linux。

## 试用方法
在目录下
```sh
g++ -std=c++11 Judger.h Judger.cpp -o Judger
g++ -std=c++11 Runner.h Runner.cpp -o runner
g++ -std=c++11 Killer.h Killer.cpp -o killer
```

在同目录下，创建一个配置文件0.in。在其中一行一个的依次填入：

1、时限

2、内存限制

3、待测可执行程序路径

4、输入数据文件路径

5、程序输出文件路径

6、标准输出文件路径

7、是否启用spj（0或1）

8、spj程序路径（如果不用就不填）

9、runner路径

10、killer路径


在目录下输入
```sh
./Judger
```
即可完成测试，评测结果在同目录下的0.out


## 正式使用

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
    const static int MAXBUF = 1e9; //数据字符最大缓存
    const static int MAX_OVERFLOW_MULTIPLE = 1000; //输出最大倍数
    const static int SPJ_MAX_TIME = 10000;
    /*
    在评测时，首先读入标准答案，然后读入待评测程序的输出，如果待评测程序的输出大小大于标准答案的MAX_OVERFLOW_MULTIPLE倍，
    即返回结果OUTPUT_LIMIT_EXCEEDED 输出超限
    */
    const static bool StrictMode = false;
    const static bool AbsolteOutputLimitMode = true;
    /*
    严格评测模式：
    如果为true，则启用严格评测模式，在严格模式下，按照以下顺序比对答案和输出：
    1、对字符串进行变换，把答案和输出数据中的"\r\n"，用"\n"替代
    2、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
    3、如果输出和答案相等，则AC。
    4、否则：将答案和输出的所有连续空白字符都用一个空格替代。
    5、如果此时输出和答案相等，则PE。
    6、否则WA。

    在非严格模式下：
    1、如果输出或者答案最后有一个'\n'，则删掉一个'\n'。
    2、将答案和输出的所有连续空白字符都用一个空格替代。
    3、如果此时输出和答案相等，则AC。
    4、否则WA。
    */
    static string ansbuf, outbuf;
    static RunConfig CreateSystemError(string me);
    static void ExecKiller(string KillerExePath, pid_t RunnerId, int KillTime);
    static void ExecRunner(string RunnerExePath, string ExePath, string InputDataPath, string OutputDataPath);
    static void ExecSPJ(string RunnerExePath, string ExePath, string InputDataPath, string OutputDataPath);
    static void GetResult(RunConfig* result, int status, rusage resource_usage);
    static void GetResultSPJ(RunConfig* result, int status, rusage resource_usage);
    static void CheckAnswer( RunConfig* result);
    static void FormatString (string &s);
    static void StrictFormatWindowsString(string & s);
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

## SPJ 说明
spj由可由c++编写，这里占用spj程序的main返回值判定情况，原则上只有AC和WA。实际上spj返回JudgerResult类中的对应的数字，可以得到包括system error在内的所有错误，但是由spj自行返回的system error不做log。
代码中spj_demo.cpp展示了一个spj程序代码的实例。（注意，JudgerConfig中的SPJExePath指的是编译过后可执行的spj程序的路径，而非代码路径。）

## 演示
演示题目：https://www.luogu.com.cn/problem/U132890

![image](https://user-images.githubusercontent.com/44296812/122661663-2edb1300-d1bf-11eb-93e4-af1061f0a74a.png)

然后在0.in中依次输入：

![image](https://user-images.githubusercontent.com/44296812/122661705-ad37b500-d1bf-11eb-9b00-2097d85c526c.png)

点击Judger运行，会在同目录下生成两个文件0.out和output.out,分别是评测结果和程序输出，其中0.out内容为：

![image](https://user-images.githubusercontent.com/44296812/122661766-862db300-d1c0-11eb-92e1-7ac85cbf5e8b.png)

## spj演示
这里来个简单的，如果标准答案和输出相差不大于1e-4就为正确。
编写待测程序
```cpp
int main()
{
	// 求解三角形第三边
	double a, b; cin >> a >> b;
	printf("%.6f\n", sqrt(a*a+b*b));

}
```
编写数据

![image](https://user-images.githubusercontent.com/44296812/122661857-43b8a600-d1c1-11eb-90fb-7f24c003fa61.png)

编写spj
```cpp
int main(int argc, char * argv[]) {
    
	FILE * input = fopen(argv[1], "r");
	FILE * output = fopen(argv[2], "r");
	FILE * stdoutput = fopen(argv[3], "r");

// Begin write code.... 
    
    	double a, b;
	fscanf(output, "%lf", &a);
	fscanf(stdoutput, "%lf", &b);
	if (abs(a - b) <= 1e-4) {
		SPJ_RESULT = ACCEPTED;
	}
	SPJ_RESULT = WRONG_ANSWER;

// End  write code.....


    fclose(input);
    fclose(output);
    fclose(stdoutput);

    return SPJ_RESULT;

}
```

同理配置

![image](https://user-images.githubusercontent.com/44296812/122662141-3c929780-d1c3-11eb-9793-c04ecf7a4cc7.png)


![image](https://user-images.githubusercontent.com/44296812/122662081-ecb3d080-d1c2-11eb-9f8b-0274e7855b3b.png)

查看结果

![image](https://user-images.githubusercontent.com/44296812/122662154-5b912980-d1c3-11eb-9618-f5b1d40b3a2c.png)

更改待测代码，重新评测：
```cpp
int main()
{
	// 求解三角形第三边
	double a, b; cin >> a >> b;
	printf("%.6f\n", sqrt(a*a+b*b)+3);

}
```
结果

![image](https://user-images.githubusercontent.com/44296812/122662191-9abf7a80-d1c3-11eb-8d2f-c27568d4d780.png)


## 只是玩具!!!


