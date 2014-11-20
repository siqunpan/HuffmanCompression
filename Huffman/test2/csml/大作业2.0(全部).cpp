/*****ͷ�ļ���*****/
#include<iostream>
#include<fstream>
#include<string>
#include<windows.h>
#include<queue>
#include<sys/stat.h>
using namespace std;

/*****�ṹ������*****/ 
struct huffmanNode                         //���������ڵ㣺����ַ���Ϣ 
{
    long long freq;                        //�ַ�Ƶ�� 
	int left, right, parent;               //���������ṹ 
	unsigned char ch;                      //ÿ���ڵ������ַ� �������и�ֵ�Ĵ�������Ҫ����Ϊunsigned char���� 
};
struct inode                               //�ļ���Ŀ¼�ڵ� 
{
    char name[1000];                       //�ļ����ļ����� 
    bool type;                             //�ļ�����        
}; 

/*****ȫ�ֱ���������*****/ 
inode* file_list=new inode[1000];          //�ļ�Ŀ¼�ڵ㣬��ʼ��Ϊ1000���ļ� 
int file_sum=0;                            //�ļ���������ʼ��Ϊ0 
FILE* wpf;                                 //�ļ��д��ʱ��������ļ����ļ�ָ�� 
FILE* rpf;                                 //��ѹ����ļ�ʱ��ȡ����ļ����ļ�ָ�� 

/*****�Ӻ���������*****/
void huffman(void);                        //���ļ��Ļ�����ѹ�����ܺ��� 
long long countWeight(long long count[], char filename[]);    //����256���ַ����ֵ�Ƶ�ʣ�����ֵΪԴ�ļ����ַ����� 
void huffmanTree(huffmanNode w[], long long count[]);         //�����ַ�Ƶ�ʽ���Huffman�� 
void huffmanCode(huffmanNode w[], char code[][256], int start[]);    //��256���ַ�����Huffman���� 
void fileWrite(huffmanNode w[], char filename[], char storename[], long long size, int start[], char code[][256]);//����Huffmanѹ�� 
void rehuffman(void);                      //���ļ���ѹ���� 
char* cut(char* partA, char* partB);       //�ڱ����ļ���Ŀ¼ʱ�����ַ�����׺ 
void findFile(char* filename);             //�ļ��б�����Ϊ�������������BFS�� 
void filePack(char* filename);       //�ļ��д���ܺ��� 
void huffmanPack(char* filename);    //�����ļ���д��ѹ�����ļ����ܺ��� 
void fileWritePack(huffmanNode w[], char filename[], long long size, int start[], char code[][256]);//�ļ���Ŀ¼��ÿ���ļ���д��ѹ�����ݺ��� 
void unpack(void);                        //��ѹ����ļ��ܺ��� 
void rehuffmanPack(void);                 //��ѹ����ļ���ÿ���ļ����� 
void fileCompare(void);                   //�ļ����ļ��бȽ��ܺ��� 
bool oneFileCompare(FILE *fp1,FILE *fp2);         //���ļ��ȽϺ���������1����Ƚϳɹ���0����ʧ�� 
bool fileListCheck(string path1,string path2);    //�ļ��бȽϺ��� ������1����Ƚϳɹ� ��0����ʧ�� 
 
/*****��������*****/
int main()
{
    int choice;                           //����ѡ����� 
    char packName[100];                   //Ҫ������ļ��� 
    char unPackName[100];     //����ɵ��ļ��� 
    
	cout<<"���������Ĺ���ѡ��0 to 5��:\n";
    cout<<"1ѹ�����ļ�;2��ѹ���ļ�;3�ļ��д����4��ѹ������ļ��У�5�ļ��бȽϣ� 0�˳�����"<<"\n";
	cin>>choice;                          //���빦��ѡ�� 
	while(choice != 0)                    //ѭ����ֹ������������ֹ��0 
	{
	    switch(choice)
        {
            case 1:                       //���е��ļ�Huffmanѹ�� 
                huffman();
            break;
            case 2:                       //���е��ļ���ѹ
                rehuffman();              
            break;
            case 3:                       //�����ļ��д�� 
                 cout<<"��������Ҫ������ļ�����"<<"\n";
                 cin>>file_list[0].name;                     //�����������ļ��� 
                 cout<<"���������ɵ��ļ���"<<"\n";    
                 cin>>packName;                       
                 wpf = fopen(packName, "wb");
                 filePack(file_list[0].name);       //�����ļ���Ŀ¼������� 
                 fclose(wpf);                                //�ر��ļ�
            break;
            case 4:
                cout<<"��������Ҫ��ѹ���ļ�����"<<"\n";
                cin>>unPackName;                             //�������Ҫ��ѹ���ļ���Ŀ¼��
                rpf = fopen(unPackName, "rb"); 
                unpack();                                    //���ý�ѹ�ļ���Ŀ¼���� 
                fclose(rpf);                                 //�ر��ļ�
            break;
            case 5:
               fileCompare();                                //���õ��ļ����ļ��бȽϺ��� 
            break;
            case 0:break;                                    //����������� 
            default:break;                        
        }
        cout<<"���������Ĺ���ѡ��0 to 5��:\n";
        cout<<"1ѹ�����ļ�;2��ѹ���ļ�;3�ļ��д����4��ѹ������ļ��У�5�ļ��бȽϣ� 0�˳�����"<<"\n";
        cin>>choice;  
	}
	system("pause");
    return 0;
	
}

/*****�Ӻ���������*****/ 
void huffman(void)                               //���ļ��Ļ�����ѹ�����ܺ������� 
{
    huffmanNode T[512];                          //Huffman���ڵ㶨�壺0-255ΪҶ�ڵ㣬256-510�Ǹ����ڵ� 
	long long size;                              //Դ�ļ��ַ��ܸ��� 
	long long count[256];                        //256���ַ���Ƶ������ 
	char filename[1000], storename[1000];        //filename:��Ҫ��ѹ�ĵ��ļ� 
	char code[256][256];                         //���256���ַ���Huffman����
	int start[256];                              //���256���ַ�Huffman�������ʼλ�ã����ڱ�������� 

	cout<<"������Ҫѹ�����ļ���:\n";             //����Ҫѹ�����ļ��� 
	cin>>filename;
	cout<<"������ѹ�������ɵ�ѹ���ļ���:\n";     //����ѹ�������ɵ��ļ��� 
	cin>>storename;
	size = countWeight(count, filename);         //����ÿ���ַ���Ƶ�ʲ������ļ��ܵ��ַ����� 
	huffmanTree(T, count);                       //���ݸ��ַ�Ƶ�ʽ���Huffman�� 
	huffmanCode(T, code, start);                 //�Ը��ַ�����Huffman���� 
	fileWrite(T, filename, storename, size, start, code);     //����ѹ������ѹ����Ϣд��ѹ���ļ� 
}
long long countWeight(long long count[], char filename[])    //����256���ַ����ֵ�Ƶ�ʣ�����ֵΪԴ�ļ����ַ�����
{
    int i=0, k=0;          
	unsigned char c;                             //���ڴ�Ÿ��ַ� Ϊunsigned char���� 
	long long size = 0;                          //����ļ����ַ����� 
	FILE *rf;                                    //Դ�ļ�ָ�� 

	rf = fopen(filename, "rb");                  //��Դ�ļ� 
	if(rf == NULL)
	{
	    cout<<"���ܴ򿪸��ļ�.";
		exit(0);
	}
	for(i = 0; i <= 255; i++)                    //��Ƶ�������ʼ�� 
		count[i] = 0;
	c = fgetc(rf);                               //��Դ�ļ� 
	while(!feof(rf))                             //�ļ�����Ϊ��ֹ���� 
	{                                 
		k = c;                                   //�ַ���ǿ������ת��������
	    count[k]++;                  
		size++; 
		c = fgetc(rf);
	}
	fclose(rf);
	return size;
}
void huffmanTree(huffmanNode w[], long long count[])     //�����ַ�Ƶ�ʽ���Huffman���������� 
{
	int i, j, p1=0, p2=0;
	long long l1, l2;                               

	for(i = 0; i < 511; i++)                     //��Huffman�����ڵ��ʼ�� 
	{
		w[i].left = -1;
		w[i].right = -1;
		w[i].parent = -1;
	}
	for(i = 0; i < 256; i++)                     //���ַ�Ƶ�ʺ��ַ�д�����ĸ�Ҷ�ڵ㣨0-255�� 
	{
		w[i].freq = count[i];
	    w[i].ch = i;
	}
	for(i = 256; i < 511; i++)                   //����Huffman�����֣�256-510Ϊ�����ڵ㣩   
	{
	    l1 = l2 = 1000000000000000LL;      //����ʱ����l1,l2��Ϊ�����ӽ�long�͵����ֵ��������dev���޷���ʼ����long long��С�� 
		for(j = 0; j < i; j++)                   //Ѱ�ҹ����ڵ���Ƶ����С���ַ���Ϊ���� 
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
		for(j = 0; j < i; j++)                   //Ѱ�ҹ����ڵ���Ƶ����С�Ҳ�ͬ�����������ַ����ַ���Ϊ�Һ��� 
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
		w[p1].parent = i;                        //����ѡ�����ڵ����Huffman���� 
		w[p2].parent = i;
		w[i].left = p1;
		w[i].right = p2;
		w[i].freq = w[p1].freq + w[p2].freq;
	}
	w[510].parent = -1;                          //���ø��ڵ� 
}
void huffmanCode(huffmanNode w[], char code[][256], int start[])    //��256���ַ�����Huffman���뺯������ 
{
	int c, p, i, sta;                            //sta������ʱ��Ÿ��ַ��������ʼλ�� 
	 
    for(i = 0; i < 256; i++)          //���벿�֣��ҵı����㷨�Ǵ���������һλ��ǰ���룬��Ҷ�ڵ�����ڵ������
	{                                 //��Ǳ�����ʼλ�ã������������code�����и��ַ��������˳������ǵ���� 
	    sta = 256;
		c = i;
		p = w[i].parent;
		while(p != -1)
		{
			if(w[p].left == c)
				code[i][--sta] = '0';     //�ҵ�Huffman����Ϊ�ַ��� 
			else
				code[i][--sta] = '1';     //�ҵ�Huffman����Ϊ�ַ��� 
			c = p;
			p = w[p].parent;
			start[i] = sta;           //��ֵ���ַ�Huffman������code�����е���ʼλ�� 
		}

	}
}
void fileWrite(huffmanNode w[], char filename[], char storename[], long long size, int start[], char code[][256])  //����Huffmanѹ���������� 
{
    int i=0, k, count2=0;
	unsigned char c, chtemp = '\0';
	FILE *rf, *wf;
    char ch = ' ';

	rf = fopen(filename, "rb");
	wf = fopen(storename, "wb");
    if(rf == NULL)
	{
	    cout<<"���ܴ򿪸��ļ�.";
		exit(0);
	}
    while(filename[count2] != '\0')       //����Դ�ļ��ļ������� 
	    count2++;
	fprintf(wf, "%d", count2);            //��Դ�ļ��ļ�������д��ѹ���ļ� 
	for(i = 0; i < count2; i++)           //��Դ�ļ���д��ѹ���ļ� 
        fprintf(wf, "%c", filename[i]);
    for(i = 0; i < 256; i++)              //�����ַ�Ƶ��д��ѹ���ļ���Ϊ�˽�ѹʱ�ؽ�Huffman�� 
	{
		fprintf(wf, "%lld", w[i].freq);
	    fprintf(wf, "%c", ch);            //��Ƶ��֮���ÿո�ֿ� 
	}
	fprintf(wf, "%lld", size);            //��Դ�ļ���Сд��ѹ���ļ� 
	i = 0;
	c = fgetc(rf);                        //��ȡԴ�ļ�����ѹ�� 
	while(!feof(rf))
	{
		for(k = start[c]; k < 256; k++)   //�������ַ���Huffman���� 
		{
			if(i != 8)                             //����ȡ���벻��8�� 
			{
			    switch(code[c][k]) 
			    {
			        case '0':
                        chtemp = chtemp*2+0;      //����λ���㣬chtemp�İ�λ������������1λ 
					    i++;
				    break;
				    case '1':
					    chtemp = chtemp*2+1;      //����λ���㣬chtemp�İ�λ������������1λ��������1 
					    i++;
				    break;
		    	}
			} 
			else                                  //���Ѷ�ȡ8�����룬chtemp��λ������д���ַ� 
			{
			    fwrite(&chtemp, 1, 1, wf);        //�����ɵ��ַ�д��ѹ���ļ� 
				i = 0;
				chtemp = '\0';                    //chtemp���³�ʼ�� 
				k--;     
			}
		}
		c = fgetc(rf);
	}
	if(i != 8 && i != 0)                          //���������ļ�����ȡ�ַ�����8�����Ҵ���0��ʱ���в�0 
	{
	    for(k = i; k <=8; k++)                    //����λ��0���� 
			chtemp = chtemp*2;
	    fwrite(&chtemp, 1, 1, wf);                //����0���ַ�д��ѹ���ļ� 
	}
	fclose(rf);                                   //�ر��ļ� 
	fclose(wf);
}
void rehuffman(void)                              //���ļ���ѹ�������� 
{    
    char filename[1000], sourcename[1000], blank;                  
	huffmanNode t[512];                           //Huffman���ڵ� 
	huffmanNode temp;            
	int countt;                      
	long long i, j, fre[256]={0}, size=0;
	unsigned char ch, ctemp; 
	FILE *rf;                                     //ѹ���ļ�ָ�� 
	FILE *sf;                                     //��ѹ�������ļ���ָ�� 
	
    cout<<"������Ҫ��ѹ���ļ���:\n";
    cin>>filename;
	rf = fopen(filename, "rb");
    if(rf == NULL)
	{
	    cout<<"���ܴ򿪸��ļ�.\n";
		exit(0);
	}
	fscanf(rf, "%d", &countt);                    //��ѹ���ļ��ж���Դ�ļ������ȣ�����ֵ��countt 
	for(i = 0; i < countt; i++)                   //��ѹ���ļ��ж���Դ�ļ���,����sourcename�ַ����� 
	    sourcename[i] = fgetc(rf);       
	sourcename[countt] = '\0';
	sf = fopen(sourcename, "wb");                 //�򿪽�ѹ�����ɵ��ļ� 
	for(i = 0; i < 256; i++)                      //��ѹ���ļ��ж������ַ���Ƶ�ʣ�����ֵ��fre���� 
	{
	    fscanf(rf, "%lld", &fre[i]);
		blank = fgetc(rf);
	}
	huffmanTree(t, fre);                          //����Ƶ�������ؽ�Huffman�� 
	temp = t[510];                                //��temp����Ϊ���ڵ� 
	fscanf(rf, "%lld", &size);                    //��ѹ���ļ��ж���Դ�ļ����ַ�����������ֵ��size 
	i = 0;
	while(i < size)                               //��ѹ���ļ����н�ѹ��ѭ����ֹ��־λ�Ѿ���ѹ��Դ�ļ����ȣ�size�����ַ� 
	{
	    ch = fgetc(rf);                            
		for(j = 7; j >= 0; j--)                   //��λ���㽫�����ַ���8λ�����������ƣ���λ��������������������Huffman��
		{                                         //ֱ��������Ҷ�ڵ㣬�����ɵ��ַ�temp.chд��Դ�ļ� 
		    ctemp = ((ch>>j)&1)+'0';               
            if(ctemp == '1')                      
				temp = t[temp.right];
			else
				temp = t[temp.left];
			if(temp.left == -1 && temp.right == -1)   //������Ҷ�ڵ� 
			{
			    fwrite(&temp.ch, 1, 1, sf);           //�����ɵ�Դ�ļ��ַ�д���ѹ���ɵ��ļ� 
				temp = t[510];
				i++;
			}
		}    
	}
	fclose(rf);                                       //�ر��ļ� 
	fclose(sf);
}
char* cut(char* partA, char* partB)               //�ڱ����ļ���Ŀ¼ʱ�����ַ�����׺�������� 
{ 
    char* ptr=partA;
    
    while(*partA!='\0')                          //�ַ�ָ��partA���Ƶ��ַ���ĩβ 
        partA++;                                  
    while(*partB!='\0')                          //�ַ�ָ��partB���Ƶ��ַ���ĩβ 
        partB++;                                  
    while(*partA == *partB)                      //���ַ���partAĩβ��partB�ַ�����ͬ�Ĳ���ɾ�� 
    {
        *partA = '\0';
        partA--;
        partB--;
    }
    return ptr;                                  //������ȥ��partAĩβ��partB�ַ������ַ�ָ�� 
}
void findFile(char* filename)
{
    WIN32_FIND_DATA FindFileData;                //�ļ��ṹ�������ļ��ĸ������� 
    HANDLE hFind;                                //���          
    int i=0;
    char *ptr;
   
    strcat(filename,"\\*.*");                    //���ļ������ֺ����ͨ���*���������ļ� 
    hFind=FindFirstFile(filename,&FindFileData); //�����ļ�·���Ѵ������ļ���������Զ�ȡ��WIN32_FIND_DATA�ṹ��ȥ,����ֵΪ�ļ���� 
    filename=cut(filename,"\\*.*");              //����cut()������ȥ���ļ�������ĩβ��"\\*.*"���� 
    FindNextFile(hFind,&FindFileData);           //�����˾��(hFind)����һ���ļ�,����ֵΪ�ļ���� 
    while(FindNextFile(hFind,&FindFileData))     //��ֹ����Ϊ���������˲��ļ���Ŀ¼ 
    {
        i++;
        ptr=file_list[file_sum+i].name;          //���ļ����ַ�ָ�븳ֵ��ptr 
        strcat(ptr,filename);                    //���ļ������ַ�������ptr 
        strcat(ptr,"\\");                        //���ַ���ptrĩβ����ַ���"\\" 
        strcat(ptr,FindFileData.cFileName);      //���ַ���ptrĩβ������ļ������Դ�����þ���·�� 
        if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) //�ж����ļ��� 
            file_list[file_sum+i].type=1;
        else                                     //�������ļ� 
            file_list[file_sum+i].type=0;
    } 
    file_sum=file_sum+i;                         //�����ܵ��ļ�������Ϊȫ�ֱ��� 
    FindClose(hFind);                            //�رվ�� 
}
void filePack(char* filename)  //�ļ��д���ܺ������� 
{
    int i,j,count=0;                              
    char ch = ' ';
    
    file_list[0].type=1;                         //���ļ��и�Ŀ¼���ͽ��г�ʼ�� 
    strcpy(file_list[0].name, filename);         //���ļ��и�Ŀ¼���ֽ��г�ʼ��
    for (i=0;i<file_sum+1;i++)                   //����BFS��������������������ļ���Ŀ¼�����г�ʼ�� 
       if (file_list[i].type==1)                 //������ļ�����Ϊ�ļ���Ŀ¼����������ļ��� 
           findFile(file_list[i].name);
    fprintf(wpf, "%d", file_sum);                //���ܵ��ļ�����д����������ɵ��ļ��� 
    fprintf(wpf, "%c", ch);                      //�ÿո���� 
    for (i=0;i<file_sum+1;i++)                   //�����ļ��ڵ���Ϣд��������ɵ��ļ���
    { 
        fprintf(wpf, "%d", file_list[i].type);   //д�����ļ����� 
        fprintf(wpf, "%c", ch);                  //�ÿո���� 
        while(file_list[i].name[count] != '\0')  //������ļ������� 
	        count++;
	    fprintf(wpf, "%d", count);               //�����ļ�������д�������ɵ��ļ��� 
	    for(j = 0; j < count; j++)               //�����ļ���д�������ɵ��ļ��� 
            fprintf(wpf, "%c", file_list[i].name[j]);
        fprintf(wpf, "%c", ch);                  //�ÿո����     
        count = 0;
    }
    for (i = 0; i <= file_sum; i++)              //��Ŀ¼�����еĵ��ļ�����ѹ�� 
        if (file_list[i].type == 0)
            huffmanPack(file_list[i].name);  //���ô���ļ��еĵ��ļ�ѹ������ 
}
void huffmanPack(char* filename)       //�����ļ���д��ѹ�����ļ����ܺ����Ķ��� 
{
    huffmanNode T[512];                          //Huffman���ڵ㶨�壺0-255ΪҶ�ڵ㣬256-510�Ǹ����ڵ�
	long long size;                              //Ŀ¼�иô�ѹ���ļ����ַ��ܸ���
	long long count[256];                        //256���ַ���Ƶ������ 
	char code[256][256];                         //��Ÿ��ڵ�Huffman����
	int start[256];                              //��Ÿ��ڵ�Huffman�������ʼλ�ã����ڼ�����볤��

	size = countWeight(count, filename);         //����ÿ���ַ���Ƶ�ʲ������ļ��ܵ��ַ����� 
	huffmanTree(T, count);                       //���ݸ��ַ�Ƶ�ʽ���Huffman��
	huffmanCode(T, code, start);                 //�Ը��ַ�����Huffman����
	fileWritePack(T, filename, size, start, code);  //����ѹ������ѹ����Ϣд�����ļ�
}
void fileWritePack(huffmanNode w[], char filename[], long long size, int start[], char code[][256])
{                                                //�ļ���Ŀ¼��ÿ���ļ���д��ѹ�����ݺ������� 
    int i=0, k, count2=0;
	unsigned char c, chtemp = '\0';
	FILE *rf;
    unsigned char ch = ' ';

	rf = fopen(filename, "rb");                  //�򿪴�ѹ���ļ�        
    if(rf == NULL)
	{
	    cout<<"���ܴ򿪸��ļ�.";
		exit(0);
	}
    while(filename[count2] != '\0')              //�����ļ������� 
	    count2++;       
	fprintf(wpf, "%d", count2);                  //���ļ�������д���������ļ� 
	for(i = 0; i < count2; i++)                  //���ļ���д���������ļ� 
        fprintf(wpf, "%c", filename[i]);  
    for(i = 0; i < 256; i++)                     //д���ļ��и��ַ���Ƶ�ʣ��ÿո���� 
	{
		fprintf(wpf, "%lld", w[i].freq);
	    fprintf(wpf, "%c", ch);
	}
	fprintf(wpf, "%lld", size);                  //д���ļ����ַ����� 
	i = 0;
	c = fgetc(rf);
	while(!feof(rf))                             //�����ļ����ݣ�����ѹ�� 
	{
		for(k = start[c]; k < 256; k++)          //�������ַ���Huffman����
		{
			if(i != 8)
			{
			    switch(code[c][k])
			    {
			        case '0':
                        chtemp = chtemp*2+0;     //����λ���㣬chtemp�İ�λ������������1λ
					    i++;
				    break;
				    case '1':
					    chtemp = chtemp*2+1;     //����λ���㣬chtemp�İ�λ������������1λ��������1
					    i++;
				    break;
		    	}
			}
			else
			{
			    fwrite(&chtemp, 1, 1,wpf);       //���Ѷ�ȡ8�����룬chtemp��λ������д���ַ�
				i = 0;
				chtemp = '\0';                   //chtemp���³�ʼ��
				k--;     
			}
		}
		c = fgetc(rf);
	}
	if(i != 8 && i != 0)                         //���������ļ�����ȡ�ַ�����8�����Ҵ���0��ʱ���в�0
	{
        for(k = i+1; k <=8; k++)                 
			chtemp = chtemp*2;                   //����λ��0����
	    fwrite(&chtemp, 1, 1, wpf);              //����0���ַ�д��ѹ���ļ�
	}
	fclose(rf);                                  //�ر��ļ� 
}
void unpack(void)                                //��ѹ����ļ��ܺ������� 
{
    int i,j,countt, file_sum; 
    unsigned char temp;
   
    fscanf(rpf, "%d", &file_sum);                //�Ӵ���ļ��ж�ȡ�ļ���������file_sum 
    temp = fgetc(rpf);                           //��ȡ�ո� 
    for(i = 0; i <= file_sum; i++)                
    {
        fscanf(rpf, "%d", &file_list[i].type);   //�Ӵ���ļ��ж�ȡ�ļ����Ͳ�����file_list[i].type 
        temp = fgetc(rpf);                       //��ȡ�ո�
        fscanf(rpf, "%d", &countt);              //�Ӵ���ļ��ж�ȡ�ļ������� 
	    for(j = 0; j < countt; j++)              //�Ӵ���ļ��ж�ȡ�ļ���  
	        file_list[i].name[j] = fgetc(rpf); 
	    file_list[i].name[countt] = '\0';
        temp = fgetc(rpf);                       //��ȡ�ո�
    }
    for(i = 0; i <= file_sum; i++)               //�����ļ�Ŀ¼���ͽ�ѹ�� 
    {
        if(file_list[i].type == 1)               
            CreateDirectory(file_list[i].name,NULL); //������ļ����򴴽����ļ��� 
        else
            rehuffmanPack();                         //������ļ�����н�ѹ���� 
    }
}
void rehuffmanPack(void)                         //��ѹ����ļ���ÿ���ļ��������� 
{
	huffmanNode t[512];                          //Huffman���ڵ㶨�� 
	huffmanNode temp;                    
	int countt;
	long long i,j;
	unsigned char ch, ctemp;
	char sourcename[1000];                       //����ļ��� 
    unsigned char blank; 
	FILE *sf;                                    //Ŀ¼���ļ����ļ�ָ�� 
	long long fre[256]={0},size=0;
    
	fscanf(rpf, "%d", &countt);                  //�Ӵ���ļ��ж�ȡ�ļ������� 
	for(i = 0; i < countt; i++)                  //���ļ��ж�ȡ�ļ��� 
	    sourcename[i] = fgetc(rpf);
	sourcename[countt] = '\0';
	sf = fopen(sourcename, "wb");                //�򿪸��ļ� 
	for(i = 0; i < 256; i++)                     //�Ӵ���ļ��ж�ȡ�ַ�Ƶ�� 
	{
	    fscanf(rpf, "%lld", &fre[i]);
		blank = fgetc(rpf);                      //�ո� 
	}
	huffmanTree(t, fre);                         //�ؽ�Huffman�� 
	temp = t[510];                               //�����ڵ㸳��temp 
	fscanf(rpf, "%lld", &size);                  //�Ӵ���ļ��ж�ȡ��ѹ���ļ����� 
	i = 0;
	while(i < size)                              //��ȡ����ļ��и��ļ���ѹ����Ϣ�����н�ѹ�� 
	{
	    ch = fgetc(rpf);
		for(j = 7; j >= 0&&i < size; j--)        //��λ���㽫�����ַ���8λ�����������ƣ���λ��������������������Huffman��
		{                                        //ֱ��������Ҷ�ڵ㣬�����ɵ��ַ�temp.chд��Դ�ļ�
		    ctemp = ((ch>>j)&1)+'0';
            if(ctemp == '1')
				temp = t[temp.right];
			else
				temp = t[temp.left];
			if(temp.left == -1 && temp.right == -1)  //��Ҷ�ڵ����� 
			{
			    fwrite(&temp.ch, 1, 1, sf);      //�����ɵ�Դ�ļ��ַ�д���ѹ���ɵ��ļ�
				temp = t[510];
				i++;
			}
		}    
	}
	fclose(sf);
}
void fileCompare(void)                           //�ļ����ļ��бȽ��ܺ���
{
    int num;                                     //�ȽϹ���ѡ�� 
    char fileName1[1000],fileName2[1000];        //��Ŵ��Ƚϵ��ļ��� 
    FILE *cf1,*cf2;                              //Ҫ�Ƚϵ��ļ�ָ�� 
	struct _stat fInfo;                          //�ļ��ṹ�����д洢���ļ��ĸ�����Ϣ 
	
    cout<<"������ѡ��(0-2):1���ļ��Ƚ�; 2�ļ���Ŀ¼�Ƚ�; 0�˳�\n";
    cin>>num; 
    while(num != 0)
	{		
		switch(num)
		{
			case 1:				                          //���е��ļ��Ƚ� 
				cout<<"������Ҫ�Ƚϵ��ļ�1:\n";
			 	cin>>fileName1;
			 	cf1 = fopen(fileName1,"rb");
		 	    if(cf1 == NULL)
		 	    {
					cout<<"���ܴ��ļ�1.\n";
					exit(0);
                }
			 	cout<<"������Ҫ�Ƚϵ��ļ�2:\n";
			 	cin>>fileName2;
				cf2 = fopen(fileName2,"rb");
				if(cf1 == NULL)
		 	    {
					cout<<"���ܴ��ļ�2.\n";
					exit(0);
                }
				else
				{
					if(oneFileCompare(cf1,cf2))           //���õ��ļ��ȽϺ���  
						cout<<"�ļ�1���ļ�2��ͬ.\n";
					else
			 			cout<<"�ļ�1���ļ�2����ͬ.\n";
				}
				break;
			case 2:                                       //�����ļ���Ŀ¼�Ƚ� 
				cout<<"������Ҫ�Ƚϵ��ļ���Ŀ¼1:\n";
			 	cin>>fileName1;
			 	if(_stat(fileName1,&fInfo) == -1)        //���û�ȡ�ļ�����Ϣ����������ֵΪ��������-1��ʾ��ȡ�ļ�����Ϣʧ��
	 			    cout<<"���ܴ��ļ���Ŀ¼1.\n";
			 	cout<<"������Ҫ�Ƚϵ��ļ���Ŀ¼2:\n";
			 	cin>>fileName2;
			 	if(_stat(fileName2,&fInfo) == -1)
 				    cout<<"���ܴ��ļ���Ŀ¼2.\n";
			 	else                                //��ȡ�ļ�����Ϣ�ɹ� 
				{
					if(fileListCheck(fileName1,fileName2) && fileListCheck(fileName2,fileName1))//�����ļ��бȽϺ������������� 
						cout<<"�ļ���1���ļ���Ŀ¼2��ͬ.\n";                                    //��0ֵ(1)��Ŀ¼��ͬ������ͬ 
					else
			 			cout<<"�ļ���1���ļ���Ŀ¼2����ͬ.\n";
				}
				break;
		}
		cout<<"������ѡ��(0-2):1���ļ��Ƚ�; 2�ļ���Ŀ¼�Ƚ�; 0�˳�\n";
		cin>>num;
	}     
}
bool oneFileCompare(FILE *cf1,FILE *cf2)    //���ļ��ȽϺ����������壬����1����Ƚϳɹ���0����ʧ��
{
    long long l1, l2, ch1, ch2;
    
    fseek(cf1,0,SEEK_END);                  //��cf1ָ���Ƶ��ļ�ĩβ  
    l1=ftell(cf1);                          //�ҳ��ȵ��ļ��� ������һ��ͨ�������������ļ����� 
    fseek(cf1,0,SEEK_SET);                  //���ļ�ָ���Ƶ��ļ���ͷ  
    fseek(cf2,0,SEEK_END);
    l2=ftell(cf2);
    fseek(cf2,0,SEEK_SET);
    if(l1!=l2)                              //�ж��ļ��Ƿ���� 
        return false;
    for(int i=0;i<l1;i++)                   //���������ļ�����ַ����бȽ�                    
    {
        ch1=fgetc(cf1);
        ch2=fgetc(cf2);
        if(ch1!=ch2)
            return false;
    }
    return true;                            //�Ƚϳɹ� 
}
bool fileListCheck(string path1,string path2)
{
    string temp, nPath1, nPath2;
    FILE *cf1, *cf2; 
    queue<pair<string,string> > Q;      //�����������������һ��������string�������ݵ����� 
    pair<string, string> path;           //һ���������ͣ������Ͷ������������ݣ�����string���� 
    struct _stat fInfo;                  //�ļ��ṹ�����д洢���ļ��ĸ�����Ϣ
    WIN32_FIND_DATA FileData;           //�ļ� ָ�� 
    HANDLE hand;                        //���                   
    
    Q.push(make_pair(path1,path2));     //ͨ��make_pair��ȡһ��string�������ݣ�֮��ѹ��Q���� 
    while(!Q.empty())                   //�����в���ʱ 
    {
        path = Q.front();               //��ȡ�������� 
        Q.pop();                        //�����ݵ������� 
        temp = path.first + "\\*";           //���ļ�����ͨ������Բ��� 
        hand=FindFirstFile(temp.c_str(),&FileData);   //���Ҹ�Ŀ¼�ļ��������ļ���� 
        if(hand != INVALID_HANDLE_VALUE)                 
        {
            while(FindNextFile(hand,&FileData) != 0)      //�ļ�δ�������� 
            {
	  		    if(FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)   //���ļ��� 
				{
					if(FileData.cFileName[0] == '.')                       //����'.''..'�ļ� 
					    continue;
					nPath2 = path.second + "\\" + FileData.cFileName;          //��ȡ�ڶ����ļ�Ŀ¼��һ���ļ����ļ���     
					if(_stat(nPath2.c_str(),&fInfo) == -1)                 //��ȡ�ļ���Ϣ 
					    return false;
					nPath1 = path.first+"\\" + FileData.cFileName + "\\";  //��ȡ��һ���ļ�Ŀ¼����һ���ļ����ļ���  
					nPath2 += "\\";
					Q.push(make_pair(nPath1,nPath2));                      //������Ŀ¼�µĸ�����һ���ļ�����Ϊ��������ѹ����� 
				}
				else                                                   //���ļ��� 
				{
				    nPath1 = path.first + "\\" + FileData.cFileName;      //��ȡ��һ���ļ����ļ���    
				    nPath2 = path.second + "\\" + FileData.cFileName;     //��ȡ�ڶ����ļ����ļ���
				    cf1 = fopen(nPath1.c_str(),"rb");                     //�򿪶���pair�е�һ���ļ� 
				    cf2 = fopen(nPath2.c_str(),"rb");                     //�򿪶���pair�еڶ����ļ�
				    if(cf2 == NULL)                                        
				    {
				        fclose(cf1);
				            return false;
				    }
				    if(oneFileCompare(cf1,cf2) == false)                  //���õ��ļ��ȽϺ��� 
				        return false;
				    fclose(cf1);
				    fclose(cf2);
				}
            }
        }
    }
    return true;
}
