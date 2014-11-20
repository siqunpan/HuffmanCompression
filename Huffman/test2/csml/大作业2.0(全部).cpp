/*****头文件区*****/
#include<iostream>
#include<fstream>
#include<string>
#include<windows.h>
#include<queue>
#include<sys/stat.h>
using namespace std;

/*****结构定义区*****/ 
struct huffmanNode                         //霍夫曼树节点：存放字符信息 
{
    long long freq;                        //字符频率 
	int left, right, parent;               //霍夫曼树结构 
	unsigned char ch;                      //每个节点代表的字符 ，由于有负值的存在所以要定义为unsigned char类型 
};
struct inode                               //文件夹目录节点 
{
    char name[1000];                       //文件或文件夹名 
    bool type;                             //文件类型        
}; 

/*****全局变量定义区*****/ 
inode* file_list=new inode[1000];          //文件目录节点，初始化为1000个文件 
int file_sum=0;                            //文件个数，初始化为0 
FILE* wpf;                                 //文件夹打包时打包生成文件的文件指针 
FILE* rpf;                                 //解压打包文件时读取打包文件的文件指针 

/*****子函数声明区*****/
void huffman(void);                        //单文件的霍夫曼压缩的总函数 
long long countWeight(long long count[], char filename[]);    //计算256个字符出现的频率，返回值为源文件的字符个数 
void huffmanTree(huffmanNode w[], long long count[]);         //根据字符频率建立Huffman树 
void huffmanCode(huffmanNode w[], char code[][256], int start[]);    //对256个字符进行Huffman编码 
void fileWrite(huffmanNode w[], char filename[], char storename[], long long size, int start[], char code[][256]);//进行Huffman压缩 
void rehuffman(void);                      //单文件解压函数 
char* cut(char* partA, char* partB);       //在遍历文件夹目录时减掉字符串后缀 
void findFile(char* filename);             //文件夹遍历，为广度优先搜索（BFS） 
void filePack(char* filename);       //文件夹打包总函数 
void huffmanPack(char* filename);    //向打包文件中写入压缩单文件的总函数 
void fileWritePack(huffmanNode w[], char filename[], long long size, int start[], char code[][256]);//文件夹目录中每个文件的写入压缩内容函数 
void unpack(void);                        //解压打包文件总函数 
void rehuffmanPack(void);                 //解压打包文件中每个文件函数 
void fileCompare(void);                   //文件或文件夹比较总函数 
bool oneFileCompare(FILE *fp1,FILE *fp2);         //单文件比较函数，返回1代表比较成功，0代表失败 
bool fileListCheck(string path1,string path2);    //文件夹比较函数 ，返回1代表比较成功 ，0代表失败 
 
/*****主函数区*****/
int main()
{
    int choice;                           //功能选择变量 
    char packName[100];                   //要打包的文件名 
    char unPackName[100];     //打包成的文件名 
    
	cout<<"请输入您的功能选择（0 to 5）:\n";
    cout<<"1压缩单文件;2解压单文件;3文件夹打包；4解压打包的文件夹；5文件夹比较； 0退出程序"<<"\n";
	cin>>choice;                          //输入功能选择 
	while(choice != 0)                    //循环截止条件：输入终止符0 
	{
	    switch(choice)
        {
            case 1:                       //进行单文件Huffman压缩 
                huffman();
            break;
            case 2:                       //进行单文件解压
                rehuffman();              
            break;
            case 3:                       //进行文件夹打包 
                 cout<<"请输入需要打包的文件夹名"<<"\n";
                 cin>>file_list[0].name;                     //输入需打包的文件名 
                 cout<<"输入打包生成的文件名"<<"\n";    
                 cin>>packName;                       
                 wpf = fopen(packName, "wb");
                 filePack(file_list[0].name);       //调用文件夹目录打包函数 
                 fclose(wpf);                                //关闭文件
            break;
            case 4:
                cout<<"请输入需要解压的文件夹名"<<"\n";
                cin>>unPackName;                             //输入的需要解压的文件夹目录名
                rpf = fopen(unPackName, "rb"); 
                unpack();                                    //调用解压文件夹目录函数 
                fclose(rpf);                                 //关闭文件
            break;
            case 5:
               fileCompare();                                //调用单文件或文件夹比较函数 
            break;
            case 0:break;                                    //程序结束运行 
            default:break;                        
        }
        cout<<"请输入您的功能选择（0 to 5）:\n";
        cout<<"1压缩单文件;2解压单文件;3文件夹打包；4解压打包的文件夹；5文件夹比较； 0退出程序"<<"\n";
        cin>>choice;  
	}
	system("pause");
    return 0;
	
}

/*****子函数定义区*****/ 
void huffman(void)                               //单文件的霍夫曼压缩的总函数定义 
{
    huffmanNode T[512];                          //Huffman树节点定义：0-255为叶节点，256-510是各父节点 
	long long size;                              //源文件字符总个数 
	long long count[256];                        //256个字符的频率数组 
	char filename[1000], storename[1000];        //filename:需要解压的单文件 
	char code[256][256];                         //存放256个字符的Huffman编码
	int start[256];                              //存放256个字符Huffman编码的起始位置，用于遍历其编码 

	cout<<"请输入要压缩的文件名:\n";             //输入要压缩的文件名 
	cin>>filename;
	cout<<"请输入压缩后生成的压缩文件名:\n";     //输入压缩后生成的文件名 
	cin>>storename;
	size = countWeight(count, filename);         //计算每个字符的频率并返回文件总的字符个数 
	huffmanTree(T, count);                       //根据各字符频率建立Huffman树 
	huffmanCode(T, code, start);                 //对各字符进行Huffman编码 
	fileWrite(T, filename, storename, size, start, code);     //进行压缩，将压缩信息写入压缩文件 
}
long long countWeight(long long count[], char filename[])    //计算256个字符出现的频率，返回值为源文件的字符个数
{
    int i=0, k=0;          
	unsigned char c;                             //用于存放各字符 为unsigned char类型 
	long long size = 0;                          //存放文件总字符个数 
	FILE *rf;                                    //源文件指针 

	rf = fopen(filename, "rb");                  //打开源文件 
	if(rf == NULL)
	{
	    cout<<"不能打开该文件.";
		exit(0);
	}
	for(i = 0; i <= 255; i++)                    //对频率数组初始化 
		count[i] = 0;
	c = fgetc(rf);                               //读源文件 
	while(!feof(rf))                             //文件结束为终止条件 
	{                                 
		k = c;                                   //字符型强制类型转换成整型
	    count[k]++;                  
		size++; 
		c = fgetc(rf);
	}
	fclose(rf);
	return size;
}
void huffmanTree(huffmanNode w[], long long count[])     //根据字符频率建立Huffman树函数定义 
{
	int i, j, p1=0, p2=0;
	long long l1, l2;                               

	for(i = 0; i < 511; i++)                     //对Huffman树各节点初始化 
	{
		w[i].left = -1;
		w[i].right = -1;
		w[i].parent = -1;
	}
	for(i = 0; i < 256; i++)                     //将字符频率和字符写入树的各叶节点（0-255） 
	{
		w[i].freq = count[i];
	    w[i].ch = i;
	}
	for(i = 256; i < 511; i++)                   //建立Huffman树部分（256-510为各父节点）   
	{
	    l1 = l2 = 1000000000000000LL;      //将临时变量l1,l2设为基本接近long型的最大值的整数（dev下无法初始化成long long大小） 
		for(j = 0; j < i; j++)                   //寻找孤立节点中频率最小的字符作为左孩子 
		{
			if(w[j].parent == -1)
			{
				if(w[j].freq < l1)
				{				
					l1 = w[j].freq;
				    p1 = j;
				}
			}
		}
		for(j = 0; j < i; j++)                   //寻找孤立节点中频率最小且不同于上面所找字符的字符作为右孩子 
		{
		    if(w[j].parent == -1)
			{
				if((w[j].freq < l2) && (j != p1))
				{
					l2 = w[j].freq;
					p2 = j;
				}
			}
		}
		w[p1].parent = i;                        //将所选孤立节点加入Huffman树中 
		w[p2].parent = i;
		w[i].left = p1;
		w[i].right = p2;
		w[i].freq = w[p1].freq + w[p2].freq;
	}
	w[510].parent = -1;                          //设置根节点 
}
void huffmanCode(huffmanNode w[], char code[][256], int start[])    //对256个字符进行Huffman编码函数定义 
{
	int c, p, i, sta;                            //sta用于临时存放各字符编码的起始位置 
	 
    for(i = 0; i < 256; i++)          //编码部分：我的编码算法是从数组的最后一位向前编码，从叶节点向根节点遍历，
	{                                 //标记编码起始位置，这样最后所得code数组中各字符编码就是顺序而不是倒叙的 
	    sta = 256;
		c = i;
		p = w[i].parent;
		while(p != -1)
		{
			if(w[p].left == c)
				code[i][--sta] = '0';     //我的Huffman编码为字符型 
			else
				code[i][--sta] = '1';     //我的Huffman编码为字符型 
			c = p;
			p = w[p].parent;
			start[i] = sta;           //赋值该字符Huffman编码在code数组中的起始位置 
		}

	}
}
void fileWrite(huffmanNode w[], char filename[], char storename[], long long size, int start[], char code[][256])  //进行Huffman压缩函数定义 
{
    int i=0, k, count2=0;
	unsigned char c, chtemp = '\0';
	FILE *rf, *wf;
    char ch = ' ';

	rf = fopen(filename, "rb");
	wf = fopen(storename, "wb");
    if(rf == NULL)
	{
	    cout<<"不能打开该文件.";
		exit(0);
	}
    while(filename[count2] != '\0')       //计算源文件文件名长度 
	    count2++;
	fprintf(wf, "%d", count2);            //将源文件文件名长度写入压缩文件 
	for(i = 0; i < count2; i++)           //将源文件名写入压缩文件 
        fprintf(wf, "%c", filename[i]);
    for(i = 0; i < 256; i++)              //将各字符频率写入压缩文件，为了解压时重建Huffman树 
	{
		fprintf(wf, "%lld", w[i].freq);
	    fprintf(wf, "%c", ch);            //各频率之间用空格分开 
	}
	fprintf(wf, "%lld", size);            //将源文件大小写入压缩文件 
	i = 0;
	c = fgetc(rf);                        //读取源文件进行压缩 
	while(!feof(rf))
	{
		for(k = start[c]; k < 256; k++)   //遍历该字符的Huffman编码 
		{
			if(i != 8)                             //当读取编码不足8个 
			{
			    switch(code[c][k]) 
			    {
			        case '0':
                        chtemp = chtemp*2+0;      //就是位运算，chtemp的八位二进制数左移1位 
					    i++;
				    break;
				    case '1':
					    chtemp = chtemp*2+1;      //就是位运算，chtemp的八位二进制数左移1位，并加上1 
					    i++;
				    break;
		    	}
			} 
			else                                  //当已读取8个编码，chtemp八位赋满，写入字符 
			{
			    fwrite(&chtemp, 1, 1, wf);        //将生成的字符写入压缩文件 
				i = 0;
				chtemp = '\0';                    //chtemp重新初始化 
				k--;     
			}
		}
		c = fgetc(rf);
	}
	if(i != 8 && i != 0)                          //当遍历完文件最后读取字符不足8个并且大于0个时进行补0 
	{
	    for(k = i; k <=8; k++)                    //不足位用0补足 
			chtemp = chtemp*2;
	    fwrite(&chtemp, 1, 1, wf);                //将补0后字符写入压缩文件 
	}
	fclose(rf);                                   //关闭文件 
	fclose(wf);
}
void rehuffman(void)                              //单文件解压函数定义 
{    
    char filename[1000], sourcename[1000], blank;                  
	huffmanNode t[512];                           //Huffman树节点 
	huffmanNode temp;            
	int countt;                      
	long long i, j, fre[256]={0}, size=0;
	unsigned char ch, ctemp; 
	FILE *rf;                                     //压缩文件指针 
	FILE *sf;                                     //解压后生成文件的指针 
	
    cout<<"请输入要解压的文件名:\n";
    cin>>filename;
	rf = fopen(filename, "rb");
    if(rf == NULL)
	{
	    cout<<"不能打开该文件.\n";
		exit(0);
	}
	fscanf(rf, "%d", &countt);                    //从压缩文件中读出源文件名长度，并赋值给countt 
	for(i = 0; i < countt; i++)                   //从压缩文件中读出源文件名,存入sourcename字符数组 
	    sourcename[i] = fgetc(rf);       
	sourcename[countt] = '\0';
	sf = fopen(sourcename, "wb");                 //打开解压后生成的文件 
	for(i = 0; i < 256; i++)                      //从压缩文件中读出各字符的频率，并赋值给fre数组 
	{
	    fscanf(rf, "%lld", &fre[i]);
		blank = fgetc(rf);
	}
	huffmanTree(t, fre);                          //根据频率数组重建Huffman树 
	temp = t[510];                                //将temp设置为根节点 
	fscanf(rf, "%lld", &size);                    //从压缩文件中读出源文件的字符个数，并赋值给size 
	i = 0;
	while(i < size)                               //对压缩文件进行解压，循环终止标志位已经解压出源文件长度（size）个字符 
	{
	    ch = fgetc(rf);                            
		for(j = 7; j >= 0; j--)                   //用位运算将所读字符的8位二进制码右移，逐位读出，并根据其编码遍历Huffman树
		{                                         //直到遍历到叶节点，将生成的字符temp.ch写入源文件 
		    ctemp = ((ch>>j)&1)+'0';               
            if(ctemp == '1')                      
				temp = t[temp.right];
			else
				temp = t[temp.left];
			if(temp.left == -1 && temp.right == -1)   //遍历到叶节点 
			{
			    fwrite(&temp.ch, 1, 1, sf);           //将生成的源文件字符写入解压生成的文件 
				temp = t[510];
				i++;
			}
		}    
	}
	fclose(rf);                                       //关闭文件 
	fclose(sf);
}
char* cut(char* partA, char* partB)               //在遍历文件夹目录时减掉字符串后缀函数定义 
{ 
    char* ptr=partA;
    
    while(*partA!='\0')                          //字符指针partA后移到字符串末尾 
        partA++;                                  
    while(*partB!='\0')                          //字符指针partB后移到字符串末尾 
        partB++;                                  
    while(*partA == *partB)                      //将字符串partA末尾与partB字符串相同的部分删掉 
    {
        *partA = '\0';
        partA--;
        partB--;
    }
    return ptr;                                  //返回已去除partA末尾的partB字符串的字符指针 
}
void findFile(char* filename)
{
    WIN32_FIND_DATA FindFileData;                //文件结构，存有文件的各种属性 
    HANDLE hFind;                                //句柄          
    int i=0;
    char *ptr;
   
    strcat(filename,"\\*.*");                    //在文件夹名字后加上通配符*用来查找文件 
    hFind=FindFirstFile(filename,&FindFileData); //根据文件路径把待操作文件的相关属性读取到WIN32_FIND_DATA结构中去,返回值为文件句柄 
    filename=cut(filename,"\\*.*");              //调用cut()函数，去掉文件夹名字末尾的"\\*.*"部分 
    FindNextFile(hFind,&FindFileData);           //搜索此句柄(hFind)的下一个文件,返回值为文件句柄 
    while(FindNextFile(hFind,&FindFileData))     //终止条件为遍历结束此层文件夹目录 
    {
        i++;
        ptr=file_list[file_sum+i].name;          //将文件名字符指针赋值给ptr 
        strcat(ptr,filename);                    //将文件夹名字符串赋给ptr 
        strcat(ptr,"\\");                        //在字符串ptr末尾添加字符串"\\" 
        strcat(ptr,FindFileData.cFileName);      //在字符串ptr末尾添加子文件名，以此来获得绝对路径 
        if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) //判断是文件夹 
            file_list[file_sum+i].type=1;
        else                                     //否则是文件 
            file_list[file_sum+i].type=0;
    } 
    file_sum=file_sum+i;                         //计算总的文件个数，为全局变量 
    FindClose(hFind);                            //关闭句柄 
}
void filePack(char* filename)  //文件夹打包总函数定义 
{
    int i,j,count=0;                              
    char ch = ' ';
    
    file_list[0].type=1;                         //对文件夹根目录类型进行初始化 
    strcpy(file_list[0].name, filename);         //对文件夹根目录名字进行初始化
    for (i=0;i<file_sum+1;i++)                   //采用BFS广度优先搜索遍历整个文件夹目录并进行初始化 
       if (file_list[i].type==1)                 //如果该文件类型为文件夹目录，则遍历该文件夹 
           findFile(file_list[i].name);
    fprintf(wpf, "%d", file_sum);                //将总的文件个数写进打包后生成的文件中 
    fprintf(wpf, "%c", ch);                      //用空格隔开 
    for (i=0;i<file_sum+1;i++)                   //将各文件节点信息写进打包生成的文件中
    { 
        fprintf(wpf, "%d", file_list[i].type);   //写进该文件类型 
        fprintf(wpf, "%c", ch);                  //用空格隔开 
        while(file_list[i].name[count] != '\0')  //计算该文件名长度 
	        count++;
	    fprintf(wpf, "%d", count);               //将该文件名长度写入打包生成的文件名 
	    for(j = 0; j < count; j++)               //将该文件名写入打包生成的文件中 
            fprintf(wpf, "%c", file_list[i].name[j]);
        fprintf(wpf, "%c", ch);                  //用空格隔开     
        count = 0;
    }
    for (i = 0; i <= file_sum; i++)              //对目录中所有的单文件进行压缩 
        if (file_list[i].type == 0)
            huffmanPack(file_list[i].name);  //调用打包文件中的单文件压缩函数 
}
void huffmanPack(char* filename)       //向打包文件中写入压缩单文件的总函数的定义 
{
    huffmanNode T[512];                          //Huffman树节点定义：0-255为叶节点，256-510是各父节点
	long long size;                              //目录中该待压缩文件的字符总个数
	long long count[256];                        //256个字符的频率数组 
	char code[256][256];                         //存放各节点Huffman编码
	int start[256];                              //存放各节点Huffman编码的起始位置，便于计算编码长度

	size = countWeight(count, filename);         //计算每个字符的频率并返回文件总的字符个数 
	huffmanTree(T, count);                       //根据各字符频率建立Huffman树
	huffmanCode(T, code, start);                 //对各字符进行Huffman编码
	fileWritePack(T, filename, size, start, code);  //进行压缩，将压缩信息写入打包文件
}
void fileWritePack(huffmanNode w[], char filename[], long long size, int start[], char code[][256])
{                                                //文件夹目录中每个文件的写入压缩内容函数定义 
    int i=0, k, count2=0;
	unsigned char c, chtemp = '\0';
	FILE *rf;
    unsigned char ch = ' ';

	rf = fopen(filename, "rb");                  //打开待压缩文件        
    if(rf == NULL)
	{
	    cout<<"不能打开该文件.";
		exit(0);
	}
    while(filename[count2] != '\0')              //计算文件名长度 
	    count2++;       
	fprintf(wpf, "%d", count2);                  //将文件名长度写入打包生成文件 
	for(i = 0; i < count2; i++)                  //将文件名写入打包生成文件 
        fprintf(wpf, "%c", filename[i]);  
    for(i = 0; i < 256; i++)                     //写入文件中各字符的频率，用空格隔开 
	{
		fprintf(wpf, "%lld", w[i].freq);
	    fprintf(wpf, "%c", ch);
	}
	fprintf(wpf, "%lld", size);                  //写入文件总字符个数 
	i = 0;
	c = fgetc(rf);
	while(!feof(rf))                             //遍历文件内容，进行压缩 
	{
		for(k = start[c]; k < 256; k++)          //遍历该字符的Huffman编码
		{
			if(i != 8)
			{
			    switch(code[c][k])
			    {
			        case '0':
                        chtemp = chtemp*2+0;     //就是位运算，chtemp的八位二进制数左移1位
					    i++;
				    break;
				    case '1':
					    chtemp = chtemp*2+1;     //就是位运算，chtemp的八位二进制数左移1位，并加上1
					    i++;
				    break;
		    	}
			}
			else
			{
			    fwrite(&chtemp, 1, 1,wpf);       //当已读取8个编码，chtemp八位赋满，写入字符
				i = 0;
				chtemp = '\0';                   //chtemp重新初始化
				k--;     
			}
		}
		c = fgetc(rf);
	}
	if(i != 8 && i != 0)                         //当遍历完文件最后读取字符不足8个并且大于0个时进行补0
	{
        for(k = i+1; k <=8; k++)                 
			chtemp = chtemp*2;                   //不足位用0补足
	    fwrite(&chtemp, 1, 1, wpf);              //将补0后字符写入压缩文件
	}
	fclose(rf);                                  //关闭文件 
}
void unpack(void)                                //解压打包文件总函数定义 
{
    int i,j,countt, file_sum; 
    unsigned char temp;
   
    fscanf(rpf, "%d", &file_sum);                //从打包文件中读取文件总数赋给file_sum 
    temp = fgetc(rpf);                           //读取空格 
    for(i = 0; i <= file_sum; i++)                
    {
        fscanf(rpf, "%d", &file_list[i].type);   //从打包文件中读取文件类型并赋给file_list[i].type 
        temp = fgetc(rpf);                       //读取空格
        fscanf(rpf, "%d", &countt);              //从打包文件中读取文件名长度 
	    for(j = 0; j < countt; j++)              //从打包文件中读取文件名  
	        file_list[i].name[j] = fgetc(rpf); 
	    file_list[i].name[countt] = '\0';
        temp = fgetc(rpf);                       //读取空格
    }
    for(i = 0; i <= file_sum; i++)               //遍历文件目录新型解压缩 
    {
        if(file_list[i].type == 1)               
            CreateDirectory(file_list[i].name,NULL); //如果是文件夹则创建该文件夹 
        else
            rehuffmanPack();                         //如果是文件册进行解压操作 
    }
}
void rehuffmanPack(void)                         //解压打包文件中每个文件函数定义 
{
	huffmanNode t[512];                          //Huffman树节点定义 
	huffmanNode temp;                    
	int countt;
	long long i,j;
	unsigned char ch, ctemp;
	char sourcename[1000];                       //存放文件名 
    unsigned char blank; 
	FILE *sf;                                    //目录中文件的文件指针 
	long long fre[256]={0},size=0;
    
	fscanf(rpf, "%d", &countt);                  //从打包文件中读取文件名长度 
	for(i = 0; i < countt; i++)                  //从文件中读取文件名 
	    sourcename[i] = fgetc(rpf);
	sourcename[countt] = '\0';
	sf = fopen(sourcename, "wb");                //打开该文件 
	for(i = 0; i < 256; i++)                     //从打包文件中读取字符频率 
	{
	    fscanf(rpf, "%lld", &fre[i]);
		blank = fgetc(rpf);                      //空格 
	}
	huffmanTree(t, fre);                         //重建Huffman树 
	temp = t[510];                               //将根节点赋给temp 
	fscanf(rpf, "%lld", &size);                  //从打包文件中读取待压缩文件长度 
	i = 0;
	while(i < size)                              //读取打包文件中该文件的压缩信息，进行解压缩 
	{
	    ch = fgetc(rpf);
		for(j = 7; j >= 0&&i < size; j--)        //用位运算将所读字符的8位二进制码右移，逐位读出，并根据其编码遍历Huffman树
		{                                        //直到遍历到叶节点，将生成的字符temp.ch写入源文件
		    ctemp = ((ch>>j)&1)+'0';
            if(ctemp == '1')
				temp = t[temp.right];
			else
				temp = t[temp.left];
			if(temp.left == -1 && temp.right == -1)  //遍叶节点历到 
			{
			    fwrite(&temp.ch, 1, 1, sf);      //将生成的源文件字符写入解压生成的文件
				temp = t[510];
				i++;
			}
		}    
	}
	fclose(sf);
}
void fileCompare(void)                           //文件或文件夹比较总函数
{
    int num;                                     //比较功能选项 
    char fileName1[1000],fileName2[1000];        //存放带比较的文件名 
    FILE *cf1,*cf2;                              //要比较的文件指针 
	struct _stat fInfo;                          //文件结构，其中存储着文件的各种信息 
	
    cout<<"请输入选项(0-2):1单文件比较; 2文件夹目录比较; 0退出\n";
    cin>>num; 
    while(num != 0)
	{		
		switch(num)
		{
			case 1:				                          //进行单文件比较 
				cout<<"请输入要比较的文件1:\n";
			 	cin>>fileName1;
			 	cf1 = fopen(fileName1,"rb");
		 	    if(cf1 == NULL)
		 	    {
					cout<<"不能打开文件1.\n";
					exit(0);
                }
			 	cout<<"请输入要比较的文件2:\n";
			 	cin>>fileName2;
				cf2 = fopen(fileName2,"rb");
				if(cf1 == NULL)
		 	    {
					cout<<"不能打开文件2.\n";
					exit(0);
                }
				else
				{
					if(oneFileCompare(cf1,cf2))           //调用单文件比较函数  
						cout<<"文件1与文件2相同.\n";
					else
			 			cout<<"文件1与文件2不相同.\n";
				}
				break;
			case 2:                                       //进行文件夹目录比较 
				cout<<"请输入要比较的文件夹目录1:\n";
			 	cin>>fileName1;
			 	if(_stat(fileName1,&fInfo) == -1)        //调用获取文件夹信息函数，返回值为整数返回-1表示获取文件夹信息失败
	 			    cout<<"不能打开文件夹目录1.\n";
			 	cout<<"请输入要比较的文件夹目录2:\n";
			 	cin>>fileName2;
			 	if(_stat(fileName2,&fInfo) == -1)
 				    cout<<"不能打开文件夹目录2.\n";
			 	else                                //获取文件夹信息成功 
				{
					if(fileListCheck(fileName1,fileName2) && fileListCheck(fileName2,fileName1))//调用文件夹比较函数，若都返回 
						cout<<"文件夹1与文件夹目录2相同.\n";                                    //非0值(1)则目录相同。否则不同 
					else
			 			cout<<"文件夹1与文件夹目录2不相同.\n";
				}
				break;
		}
		cout<<"请输入选项(0-2):1单文件比较; 2文件夹目录比较; 0退出\n";
		cin>>num;
	}     
}
bool oneFileCompare(FILE *cf1,FILE *cf2)    //单文件比较函数函数定义，返回1代表比较成功，0代表失败
{
    long long l1, l2, ch1, ch2;
    
    fseek(cf1,0,SEEK_END);                  //把cf1指针移到文件末尾  
    l1=ftell(cf1);                          //找长度到文件， 与上面一句通常连用用来找文件产度 
    fseek(cf1,0,SEEK_SET);                  //把文件指针移到文件开头  
    fseek(cf2,0,SEEK_END);
    l2=ftell(cf2);
    fseek(cf2,0,SEEK_SET);
    if(l1!=l2)                              //判断文件是否相等 
        return false;
    for(int i=0;i<l1;i++)                   //遍历两个文件逐个字符进行比较                    
    {
        ch1=fgetc(cf1);
        ch2=fgetc(cf2);
        if(ch1!=ch2)
            return false;
    }
    return true;                            //比较成功 
}
bool fileListCheck(string path1,string path2)
{
    string temp, nPath1, nPath2;
    FILE *cf1, *cf2; 
    queue<pair<string,string> > Q;      //队列容器，其对象是一个有两个string类型数据的类型 
    pair<string, string> path;           //一个对象类型，该类型对象有两个数据，都是string类型 
    struct _stat fInfo;                  //文件结构，其中存储着文件的各种信息
    WIN32_FIND_DATA FileData;           //文件 指针 
    HANDLE hand;                        //句柄                   
    
    Q.push(make_pair(path1,path2));     //通过make_pair获取一对string类型数据，之后压入Q队列 
    while(!Q.empty())                   //当队列不空时 
    {
        path = Q.front();               //读取队首数据 
        Q.pop();                        //将数据弹出队列 
        temp = path.first + "\\*";           //给文件加上通配符用以查找 
        hand=FindFirstFile(temp.c_str(),&FileData);   //查找根目录文件，返回文件句柄 
        if(hand != INVALID_HANDLE_VALUE)                 
        {
            while(FindNextFile(hand,&FileData) != 0)      //文件未遍历结束 
            {
	  		    if(FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)   //是文件夹 
				{
					if(FileData.cFileName[0] == '.')                       //忽略'.''..'文件 
					    continue;
					nPath2 = path.second + "\\" + FileData.cFileName;          //获取第二个文件目录下一个文件的文件名     
					if(_stat(nPath2.c_str(),&fInfo) == -1)                 //获取文件信息 
					    return false;
					nPath1 = path.first+"\\" + FileData.cFileName + "\\";  //获取第一个文件目录下下一个文件的文件名  
					nPath2 += "\\";
					Q.push(make_pair(nPath1,nPath2));                      //将两个目录下的各自下一个文件名作为对象数据压入队列 
				}
				else                                                   //是文件夹 
				{
				    nPath1 = path.first + "\\" + FileData.cFileName;      //获取第一个文件的文件名    
				    nPath2 = path.second + "\\" + FileData.cFileName;     //获取第二个文件的文件名
				    cf1 = fopen(nPath1.c_str(),"rb");                     //打开对象pair中第一个文件 
				    cf2 = fopen(nPath2.c_str(),"rb");                     //打开对象pair中第二个文件
				    if(cf2 == NULL)                                        
				    {
				        fclose(cf1);
				            return false;
				    }
				    if(oneFileCompare(cf1,cf2) == false)                  //调用单文件比较函数 
				        return false;
				    fclose(cf1);
				    fclose(cf2);
				}
            }
        }
    }
    return true;
}
