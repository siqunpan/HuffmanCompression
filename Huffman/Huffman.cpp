/*****Les en-tête*****/
#include<iostream>
#include<fstream>
#include<string>
#include<windows.h>
#include<queue>
#include<sys/stat.h>
using namespace std;

/*****Les structures*****/ 
struct huffmanNode                         //Le point de Huffman arbre pour déposer les informations du caractère
{
    long long freq;                        //Le fréquence du caractère 
    int left, right, parent;               //Le strucure de Huffman arbre 
    unsigned char ch;                      //Le caractère représenté par le point, il est "unsigned" en raison de la présence de négatif
};
struct inode                               //Le point du fichier ou répertoire
{
    char name[1000];                       //Le nom 
    bool type;                             //Le type du fichier       
}; 

/*****Les globals variables*****/ 
inode* file_list=new inode[1000];          //Les pointeurs des fichiers qui est initialisé à 1000 
int file_sum=0;                            //Le nombre du fichiers qui est initialisé à 0 
FILE* wpf;                                 //Le pointeur du fichier généré quand je emballe les fichiers
FILE* rpf;                                 //Le pointeur du fichier emballé quand je décompresse le fichier

/*****Les declarations des fonctions*****/
void huffman(void);                        //Compresser un fichier 
long long countWeight(long long count[], char filename[]);//Calculer les fréquences des 256 caractères, la valeur de retour est le nombre des caractères de fichier source
void huffmanTree(huffmanNode w[], long long count[]);     //Construire le Huffman arbre selon les frequences des caractère
void huffmanCode(huffmanNode w[], char code[][256], int start[]);    //Le codage de Huffman sur le 256 caractères
void fileWrite(huffmanNode w[], char filename[], char storename[], long long size, int start[], char code[][256]);//Compression Huffman 
void rehuffman(void);                      //Décompresser un fichier 
char* cut(char* partA, char* partB);       //Supprimer le suffixe du chaêne quand je parcours le répertoire
void findFile(char* filename);             //Parcourir le répertoire en utilisant "Breadth-First Search"(BFS) 
void filePack(char* filename);       //Emballer les fichiers  
void huffmanPack(char* filename);    //Compresser les fichiers dans le fichier emballé
void onlyWrite(char *filename, int size);     //Si le fichier est ".rar", l'écrire directement dans le fichier emballé
void reOnlyWrite(char *sourcename);           //Lire le fichier qui est ".rar" directement via le fichier emballé
void fileWritePack(huffmanNode w[], char filename[], long long size, int start[], char code[][256]);//Ecrire le contenu de chaque fichier après compression
void unpack(void);                        //Décompresser le fichier emballé
void rehuffmanPack(void);                 //Décompresser chaque fichier dans le fichier emballé
void fileCompare(void);                   //Le fonction général pour comparer les fichiers ou répertoires
bool oneFileCompare(FILE *fp1,FILE *fp2);         //Le fonction pour comparer deux fichiers, 1 représente ils sont mêmes, 0 représente ils sont différents
bool fileListCheck(string path1,string path2);    //Le fonction pour comparer deux répertoire
 
/*****Toutes les fonctions*****/
int main()
{
    char choice;                           //Choisir qu'est-ce que tu veux faire 
    char packName[100];                   //Le nom du fichier qui est besoin de être emballé 
    char unPackName[100];     //Le nom du fichier après emballer 
    
	cout<<"Entrer votre choix(0 to 5):\n";
    cout<<"       1.Compresser un fichier\n";
    cout<<"       2.Décompresser un fichier\n";
    cout<<"       3.Emballer un répertoire\n";
    cout<<"       4.Décompresser le fichier emballé\n";
    cout<<"       5.Comparer les fichiers\n";
    cout<<"       0.Exit"<<endl;
	cin>>choice;                          //Entrer le choix 
	while(choice != '0')                    //Si ne entrer pas 0 
	{
	    switch(choice)
        {
            case '1':                       //Compresser un fichier en utilisant Huffman 
                huffman();
            break;
            case '2':                       //Décompresser un fichier
                rehuffman();              
            break;
            case '3':                       //Emballer un répertoire 
                 cout<<"Entrer le nom du répertoire que vous voulez emballer:\n";
                 cin>>file_list[0].name;                     //Entrer le nom du répertoire qui va être emballé
                 cout<<"Entrer le nom du fichier généré:\n";    
                 cin>>packName;                       
                 cout<<"Patientez\n";
                 wpf = fopen(packName, "wb");
                 filePack(file_list[0].name);       //Appeler le fonction d'emballer un répertoire 
                 fclose(wpf);                                //Fermer le pointeur du fichier 
            break;
            case '4':
                cout<<"Entrer le nom du répertoire que vous voulez décompresser:\n";
                cin>>unPackName;                             //Entrer le nom du fichier qui va être décompressé 
                cout<<"Patientez\n"; 
                rpf = fopen(unPackName, "rb"); 
                unpack();                                    //Appeler le fonction de décompresser un répertoire 
                fclose(rpf);                                 //Fermer le pointeur du fichier
                file_list = NULL;
                file_sum = 0;
            break;
            case '5':
               fileCompare();                                //Appeler le fonction de comparer les fichiers 
            break;
            case '0':break;                                    //Exit 
            default:break;                        
        }
        cout<<"Entrer votre choix(0 to 5):\n";
        cout<<"       1.Compresser un fichier\n";
        cout<<"       2.Décompresser un fichier\n";
        cout<<"       3.Emballer un répertoire\n";
        cout<<"       4.Décompresser le fichier emballé\n";
        cout<<"       5.Comparer les fichiers\n";
        cout<<"       0.Exit\n";
    	cin>>choice;                          //Entrer le choix 
	}
	system("pause");
    return 0;
	
}

/*****Les définitions des fonctions*****/ 
void huffman(void)                               //Compresser un fichier 
{
    huffmanNode T[512];                          //Les points de Huffman arbre：0-255 sont les feuilles-points, 256-510 sont les parents-points 
	long long size;                              //Le nombre des caractères du fichier source 
	long long count[256];                        //Le array des frequences des 256 caractère 
	char filename[1000], storename[1000];        //filename: le fichier qu'on compressera 
	char code[256][256];                         //Les codages de Huffman pour les 256 caractères
	int start[256];                              //Les positions initiales des codages de Huffman des 256 caractères pour parcourir les codages 

	cout<<"Entrer le nom du fichier vous voulez compresser:\n";             
	cin>>filename;                               //Le nom du fichier compressé 
	cout<<"Entrer le nom du fichier généré:\n";     
	cin>>storename;                              //Le nom du fichier généré
	cout<<"Patientez\n";
	size = countWeight(count, filename);         //Calculer le fréquence de chaque caractère, le valeur de retour est le nombre des caractères du fichier
    huffmanTree(T, count);                       //Construire le Huffman Arbre selon les frequences des caractères
    huffmanCode(T, code, start);                 //Codage de Huffman pour toutes les caractère
    fileWrite(T, filename, storename, size, start, code);     //Compresser et écrire le contenu généré dans un fichier
}
long long countWeight(long long count[], char filename[])  //Calculer les fréquences des 256 caractères, 
{                                                             //le valeur de retour est le nombre des caractères du fichier
    int i=0, k=0;          
	unsigned char c;                             //Le caractère, il est "unsigned" parce que certains caractères ont les valeurs negatives
	long long size = 0;                          //Le nombre des caractères du fichier
	FILE *rf;                                    //Le pointeur du fichier source 

	rf = fopen(filename, "rb");                  //Ouvrir le fichier source 
	if(rf == NULL)
	{
	    cout<<"Ne peut pas ouvrir de fichier.";
		exit(0);
	}
	for(i = 0; i <= 255; i++)                    //Initialiser le array des fréquences 
		count[i] = 0;
	c = fgetc(rf);                               //Lire le fichier source 
	while(!feof(rf))                             //Si on lit àla fin du fichier  
	{                                 
		k = c;                                   //Transformer le type "char" à"int"
	    count[k]++;                  
		size++; 
		c = fgetc(rf);
	}
	fclose(rf);
	return size;
}
void huffmanTree(huffmanNode w[], long long count[])     //Construire le Huffman arbre selon les fréquences des caractères
{
	int i, j, p1=0, p2=0;
	long long l1, l2;                               

	for(i = 0; i < 511; i++)                     //Initialiser les points du Huffman arbre对Huffman树各节点初始化 
	{
		w[i].left = -1;
		w[i].right = -1;
		w[i].parent = -1;
	}
	for(i = 0; i < 256; i++)                     //Ecrire les fréquences des 256 caractères et les 256 caractères dans les feuilles-points
	{
		w[i].freq = count[i];
	    w[i].ch = i;
	}
	for(i = 256; i < 511; i++)                   //Construire le Huffman arbre (256-510 sont les parents-points)   
	{
	    l1 = l2 = 1000000000000000LL;            //Initialiser l1 et l2 àles valeurs maximals
		for(j = 0; j < i; j++)                   //Trouver le point isolé avec la fréquence minimale dans touts les points isolés,
		{                                          //et définir ce point comme le gauche-enfant
			if(w[j].parent == -1)
			{
				if(w[j].freq < l1)
				{				
					l1 = w[j].freq;
				    p1 = j;
				}
			}
		}
		for(j = 0; j < i; j++)                   //Trouver le point isolé différent le point trouvé avant avec la fréquence minimale
		{                                          //dans touts les points isolés,et définir ce point comme le droite-enfant
		    if(w[j].parent == -1)
			{
				if((w[j].freq < l2) && (j != p1))
				{
					l2 = w[j].freq;
					p2 = j;
				}
			}
		}
		w[p1].parent = i;                        //Ajouter les points isolés trouvé dans le Huffman arbre
		w[p2].parent = i;
		w[i].left = p1;
		w[i].right = p2;
		w[i].freq = w[p1].freq + w[p2].freq;
	}
	w[510].parent = -1;                          //Définir le racine-point 
}
void huffmanCode(huffmanNode w[], char code[][256], int start[])    //Codage de Huffman pour les 256 caractères
{
	int c, p, i, sta;                            //sta: stocker les positions initiale des codages des caractères
	 
    for(i = 0; i < 256; i++)          //Partie de codage: Mon algorithme de codage est à coder àpartir de la fin du array,
	{                                 //parcourir àpartir des feuilles-points au racine-point
	    sta = 256;                    //sta: La position initiale, donc le codage est séquentiel dans le array "code"
		c = i;
		p = w[i].parent;
		while(p != -1)
		{
			if(w[p].left == c)
				code[i][--sta] = '0';     //Le type de mes codes est "char"
			else
				code[i][--sta] = '1';      
			c = p;
			p = w[p].parent;
			start[i] = sta;           //Le array "start" stocke les positions initiales des codes Huffman 
		}

	}
}
void fileWrite(huffmanNode w[], char filename[], char storename[], long long size, int start[], char code[][256])  //Compression Huffman
{
    int i=0, k, count2=0;
	unsigned char c, chtemp = '\0';
	FILE *rf, *wf;
    char ch = ' ';

	rf = fopen(filename, "rb");
	wf = fopen(storename, "wb");
    if(rf == NULL)
	{
	    cout<<"Ne peut pas ouvrir de fichier.";
		exit(0);
	}
    while(filename[count2] != '\0')       //calculer la longueur du nom du fichier source 
	    count2++;
	fprintf(wf, "%d", count2);            //Ecrire la longueur du nom du fichier source dans le fichier généré
	for(i = 0; i < count2; i++)           //Ecrire le nom du fichier source dans le fichier généré
        fprintf(wf, "%c", filename[i]);
    for(i = 0; i < 256; i++)              //Ecrire les fréquences des caractères dans le fichier généré pour construire le Huffman arbre为了解压时重建Huffman树 
	{                                     // pour construire le Huffman arbre quand décompresser
		fprintf(wf, "%lld", w[i].freq);
	    fprintf(wf, "%c", ch);            //Les fréquences sont séparés par les espaces
	}
	fprintf(wf, "%lld", size);            //Ecrire la taille du fichier source dans le fichier généré
	i = 0;
	c = fgetc(rf);                        //Lire le fichier source pour compresser 
	while(!feof(rf))
	{
		for(k = start[c]; k < 256; k++)   //Parcourir le codage Huffman du caractère 
		{
			if(i != 8)                             //Si les caractères que je lis < 8 
			{
			    switch(code[c][k]) 
			    {
			        case '0':
                        chtemp = chtemp*2+0;      //Opération Bit: "chtemp"(il a 8 bits) se déplace un bit àgauche 
					    i++;
				    break;
				    case '1':
					    chtemp = chtemp*2+1;      //Opération Bit: "chtemp" se déplace un bit àgauche et plus 1  
					    i++;
				    break;
		    	}
			} 
			else                                  //Si les caractères que je lis == 8, les 8 bits de "chtemp" sont attribués les valeur
			{                                       //et j'écris "chtemp" dans le fichier généré  
			    fwrite(&chtemp, 1, 1, wf);        //Ecrire "chtemp" dans le fichier généré  
				i = 0;
				chtemp = '\0';                    //Initialiser "chtemp" 
				k--;     
			}
		}
		c = fgetc(rf);
	}
	if(i != 8 && i != 0)                          //Quand je lis àla fin du fichier, mais les caractère que je lis est < 8, et != 0
	{
	    for(k = i; k <=8; k++)                    //Les bits insuffisants sont attribués 0;
			chtemp = chtemp*2;
	    fwrite(&chtemp, 1, 1, wf);                //Ecrire "chtemp" dans le fichier généré après attribuer 0  
	}
	fclose(rf);                                    
	fclose(wf);
}
void rehuffman(void)                              //Décompresser un fichier
{    
    char filename[1000], sourcename[1000], blank;                  
	huffmanNode t[512];                           //Les points du Huffman arbre 
	huffmanNode temp;            
	int countt;                      
	long long i, j, fre[256]={0}, size=0;
	unsigned char ch, ctemp; 
	FILE *rf;                                     //Le pointeur du fichier après compresser
	FILE *sf;                                     //Le pointeur du fichier généré après décompresser
	
    cout<<"Entrer le nom que vous voulez décompresser:\n";
    cin>>filename;
    cout<<"Patientez\n";
	rf = fopen(filename, "rb");
    if(rf == NULL)
	{
	    cout<<"Ne peut pas ouvrir de fichier.\n";
		exit(0);
	}
	fscanf(rf, "%d", &countt);                    //Lire la longueur du nom du fichier source dans le fichier généré après compresser  
	for(i = 0; i < countt; i++)                   //Lire le nom du fichier source dans le fichier généré après compresser
	    sourcename[i] = fgetc(rf);       
	sourcename[countt] = '\0';
	sf = fopen(sourcename, "wb");                 //Ouvrir le fichier généré après décompresser
	for(i = 0; i < 256; i++)                      //Lire les fréquences des 256 caractères dans le fichier généré après compresser
	{
	    fscanf(rf, "%lld", &fre[i]);
		blank = fgetc(rf);
	}
	huffmanTree(t, fre);                          //Reconstruire le Huffman arbre selon les fréquences
	temp = t[510];                                //"temp" est défini comme le racine-point 
	fscanf(rf, "%lld", &size);                    //Lire le nombre des caractères du fichier source
	i = 0;
	while(i < size)                               //Décompression 
	{
	    ch = fgetc(rf);                            
		for(j = 7; j >= 0; j--)                   //Déplacer les 8 bits du caractère àdroite en utilisant bit opération逐
		{                                           //et lire chaque bit dans ce caractère jusqu'a on parcourt la feuille-point, et écrire le caractère généré
		    ctemp = ((ch>>j)&1)+'0';                //après écrire le caractère généré "temp.ch" dans le fichier source 
            if(ctemp == '1')                      
				temp = t[temp.right];
			else
				temp = t[temp.left];
			if(temp.left == -1 && temp.right == -1)   //Parcourir jusqu'à la feuille 
			{
			    fwrite(&temp.ch, 1, 1, sf);           //Ecrire le caractère généré dans le fichier généré après  décompresser 
				temp = t[510];
				i++;
			}
		}    
	}
	fclose(rf);                                        
	fclose(sf);
}
char* cut(char* partA, char* partB)              //Supprimer le suffixe du chaêne quand je parcours le répertoire
{ 
    char* ptr=partA;
    
    while(*partA!='\0')                          //Déplacer le pointeur "partA" à la fin de la cha?ne 
        partA++;                                  
    while(*partB!='\0')                          //Déplacer le pointeur "partB" à la fin de la cha?ne
        partB++;                                  
    while(*partA == *partB)                      //Supprimer la même partie de la fin de la cha?ne  "partA" et "partB"
    {
        *partA = '\0';
        partA--;
        partB--;
    }
    return ptr;                                  //Retourner le pointeur de la cha?ne "partA" qui est supprimée la cha?ne "partB" dans sa fin 
}
void findFile(char* filename)
{
    WIN32_FIND_DATA FindFileData;                //La structure du fichier, elle consiste les propriétéd'un fichier
    HANDLE hFind;                                //Poignée          
    int i=0;
    char *ptr;
   
    strcat(filename,"\\*.*");                    //Ajouter le caractère générique "*" après le nom du répertoire pour chercher les fichier
    hFind=FindFirstFile(filename,&FindFileData); //Ecrire les propriétés du fichier dans WIN32_FIND_DATA selon son chemin, le valeur de retour est la poignée  
    filename=cut(filename,"\\*.*");              //Supprimer la partie "\\*.*" dans la fin du répertoire 
    FindNextFile(hFind,&FindFileData);           //Chercher le fichier prochaine dans la poignée(hFind), le valeur de retour est l'autre poignée
    while(FindNextFile(hFind,&FindFileData))     //Le condition de terminaison est àparcourir touts les fichier dans ce degré du répertoire
    {
        i++;
        ptr=file_list[file_sum+i].name;          //Attribuer le pointeur du nom du fichier à"ptr" 
        strcat(ptr,filename);                    //Attribuer le nom du répertoire au "ptr" 
        strcat(ptr,"\\");                        //Ajouter "\\" à la fin du "ptr" 
        strcat(ptr,FindFileData.cFileName);      //Ajouter le nom du enfant-fichier àla fin du "ptr" pour obtenir le chemin absolu
        if(FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) //Déterminer si il est un répertoire 
            file_list[file_sum+i].type=1;
        else                                     //Il est un fichier 
            file_list[file_sum+i].type=0;
    } 
    file_sum=file_sum+i;                         //Calculer le nombre des fichiers 
    FindClose(hFind);                            //Fermer la poignée
}
void filePack(char* filename)  //Emballer le répertoire
{
    int i,j,count=0;                              
    char ch = ' ';
    
    file_list[0].type=1;                         //Initialiser le type du racine-répertoire 
    strcpy(file_list[0].name, filename);         //Initialiser le nom du racine-répertoire
    for (i=0;i<file_sum+1;i++)                   //Parcourir tout le répertoire en utilisant BFS 
       if (file_list[i].type==1)                 //Si le type est "parcourir", parcourir ce répertoire
           findFile(file_list[i].name);
    fprintf(wpf, "%d", file_sum);                //Ecrire le nombre du fichiers dans le fichier généré  après emballer  
    fprintf(wpf, "%c", ch);                      //Séparer en utilisant le espace 
    for (i=0;i<file_sum+1;i++)                   //Ecrire les informations des fichiers-points dans le fichier généré  après emballer
    { 
        fprintf(wpf, "%d", file_list[i].type);   //Ecrire le type du fichier 
        fprintf(wpf, "%c", ch);                  //Séparer en utilisant le espace 
        while(file_list[i].name[count] != '\0')  //Calculer la longueur du nom du fichier
	        count++;
	    fprintf(wpf, "%d", count);               //Ecrire la longueur du nom du fichier
	    for(j = 0; j < count; j++)               //Ecrire le nom du fichier
            fprintf(wpf, "%c", file_list[i].name[j]);
        fprintf(wpf, "%c", ch);                  //Séparer en utilisant le espace     
        count = 0;
    }
    for (i = 0; i <= file_sum; i++)              //Compresser tous les fichiers dans le répertoire 
        if (file_list[i].type == 0)
            huffmanPack(file_list[i].name);  //Appeler la fonction de la compression d'un fichier dans le répertoire 
}
void huffmanPack(char* filename)       //Compresser d'un fichier dans le répertoire
{
    huffmanNode T[512];                          //Les points du Huffman arbre, 0-255 sont feuilles-points, 256-510 sont parents-points 
	long long size;                              //Le nombre du caractères des tous les fichiers dans le répertoire
	long long count[256];                        //Le array des fréquences des 256 caractères
	char code[256][256];                         //Les codes Huffman des 256 caractères 
	int start[256];                              //Les positions initiales des codes Huffman des 256 caractères
    int count3=0,i,flag=0;
    char *rar = ".rar";
    char *zip = ".zip"; 
    FILE *wf; 
    bool isEmpty = false;
    
    while(filename[count3] != '.')              //Calculer la longueur du nom du fichier
	    count3++;
    for(i = 0; filename[count3+i]!='\0' && rar[i]!='\0'; i++)    //Si il est le fichier compressé
    {
        if(filename[count3+i] != rar[i])
            flag = 1;
    }
    for(i = 0; filename[count3+i]!='\0' && zip[i]!='\0'; i++)    //Si il est le fichier compressé
    {
        if(filename[count3+i] != zip[i])
            flag = 1;
    }
	size = countWeight(count, filename);         //Calculer les fréquences des caractères, le valeur de retour est la taille du fichier 

    if(size == 0){
        isEmpty = true;
        wf = fopen(filename, "wb");                  //Ouvrir le fichier source 
	    if(wf == NULL)
	    {
	       cout<<"Ne peut pas ouvrir de fichier.";
	       	exit(0);
       	}
       	fprintf(wf, "%s", "empty"); 
       	fclose(wf);
    } 
    size = countWeight(count, filename);

    if(flag == 1)                                   //Il n'est pas de fichier compressé
	{
        huffmanTree(T, count);                       //Construire le Huffman arbre selon les fréquences des caractères
	    huffmanCode(T, code, start);                 //Codage Huffman pour chaque caractère
	    fileWritePack(T, filename, size, start, code);  //Compresser, et écrire les informations dans le fichier généré après emballer 
	    if(isEmpty){                                    //Vider le fichier 
            wf = fopen(filename, "w");                  //Ouvrir le fichier source 
    	    if(wf == NULL)
    	    {
    	       cout<<"Ne peut pas ouvrir de fichier.";
    	       	exit(0);
           	}
           	fclose(wf);
        }
    }
    else                                             //Il est le fichier compressé, donc seulement écrire  
        onlyWrite(filename, size);                   //Seulement écrire, pas besoin de compresser  
}
void fileWritePack(huffmanNode w[], char filename[], long long size, int start[], char code[][256])
{                                                //Ecrire le contenu de compression d'un fichier dans le répertoire  
    int i=0, k, count2=0;
	unsigned char c, chtemp = '\0';
	FILE *rf;
    unsigned char ch = ' ';

	rf = fopen(filename, "rb");                  //Ouvrir le fichier qui est besoin de être compressé        
    if(rf == NULL)
	{
	    cout<<"Ne pout pas ouvrir de fichier.";
		exit(0);
	}
    while(filename[count2] != '\0')               
	    count2++;       
    //cout<<"fileWritePack():countofname: "<<count2<<endl;
   // cout<<"fileWritePack():filename: "<<filename<<endl;
	fprintf(wpf, "%d", count2);                  //Ecrire la longueur du nom du fichier
	for(i = 0; i < count2; i++)                  //Ecrire le nom du fichier
        fprintf(wpf, "%c", filename[i]);  
    for(i = 0; i < 256; i++)                     //Ecrire les fréquences des caractères, séparés par les espaces 
	{
		fprintf(wpf, "%lld", w[i].freq);
	    fprintf(wpf, "%c", ch);
	}
	fprintf(wpf, "%lld", size);                  //Ecrire la taille du fichier 
	i = 0;
	c = fgetc(rf);
	while(!feof(rf))                             //Parcourir le fichier et le compresser
	{
		for(k = start[c]; k < 256; k++)          //Parcourir le codage Huffman du caractère
		{
			if(i != 8)                           //Si les caractères que je lis < 8 
			{
			    switch(code[c][k])
			    {
			        case '0':
                        chtemp = chtemp*2+0;     //Opération Bit: "chtemp"(il a 8 bits) se déplace un bit àgauche 
					    i++;
				    break;
				    case '1':
					    chtemp = chtemp*2+1;     //Opération Bit: "chtemp" se déplace un bit àgauche et plus 1 
					    i++;
				    break;
		    	}
			}
			else                                 //Si les caractères que je lis == 8, les 8 bits de "chtemp" sont attribués les valeur
			{                                      //et j'écris "chtemp" dans le fichier généré  
			    fwrite(&chtemp, 1, 1,wpf);       //Ecrire "chtemp" dans le fichier généré
				i = 0;
				chtemp = '\0';                   //Initialiser "chtemp"
				k--;     
			}
		}
		c = fgetc(rf);
	}
	if(i != 8 && i != 0)                         //Quand je lis àla fin du fichier, mais les caractère que je lis est < 8, et != 0
	{
        for(k = i+1; k <=8; k++)                 //Les bits insuffisants sont attribués 0;
			chtemp = chtemp*2;                  
	    fwrite(&chtemp, 1, 1, wpf);              //Ecrire "chtemp" dans le fichier généré après attribuer 0  
	}
	fclose(rf);                                  //Fermer le fichier 
}
void unpack(void)                                //Décompresser le fichier emballé  
{
    int i,j,countt, file_sum; 
    unsigned char temp;
   
    fscanf(rpf, "%d", &file_sum);                //Lire le numbre des fichiers dans le fichier généré après emballer 
    temp = fgetc(rpf);                           //Lire l'espace 
    for(i = 0; i <= file_sum; i++)                
    {
        fscanf(rpf, "%d", &file_list[i].type);   //Lire le type du fichier
        temp = fgetc(rpf);                       //Lire l'espace
        fscanf(rpf, "%d", &countt);              //Lire la longueur du nom du fichier
	    for(j = 0; j < countt; j++)              //Lire le nom du fichier 
	        file_list[i].name[j] = fgetc(rpf); 
	    file_list[i].name[countt] = '\0';
        temp = fgetc(rpf);                       //Lire l'espace 
    }
    for(i = 0; i <= file_sum; i++)               //Parcourir le répertoire pour compresser
    {
        if(file_list[i].type == 1)               
            CreateDirectory(file_list[i].name,NULL); //Si il est un répertoire, le créer 
        else
            rehuffmanPack();                         //Si il est un fichier, le compresser
    }
}
void rehuffmanPack(void)                         //Décompresser un fichier dans le répertoire 
{
	huffmanNode t[512];                          //Les points du Huffman arbre 
	huffmanNode temp;                    
	int countt;
	long long i,j;
	unsigned char ch, ctemp;
	char sourcename[1000];                       //Le nom du fichier 
    unsigned char blank; 
	FILE *sf;                                    //Le pointeur du fichier dans le répertoire
	long long fre[256]={0},size=0;
    int count3=0,flag=0;
    char *rar = ".rar";
    char *zip = ".zip"; 
    char isEmpty[5];
    
	fscanf(rpf, "%d", &countt);                  //Lire la longueur du fichier
	for(i = 0; i < countt; i++)                  //lire le nom du fichier 
	    sourcename[i] = fgetc(rpf);
	sourcename[countt] = '\0';
    
    while(sourcename[count3] != '.')              //Déterminer si le fichier est déjà compressé   
	    count3++;
    for(i = 0; sourcename[count3+i]!='\0' && rar[i]!='\0'; i++)
    {
        if(sourcename[count3+i] != rar[i])
            flag = 1;
    }
    for(i = 0; sourcename[count3+i]!='\0' && zip[i]!='\0'; i++)
    {
        if(sourcename[count3+i] != zip[i])
            flag = 1;
    }
	if(flag == 0)                                    //Si déjà compressé, lire le contenu directement 
        reOnlyWrite(sourcename);                     //Ecrire le contenu au fichier source 
	else                                             //Sinon, le compresser 
    {
	    sf = fopen(sourcename, "wb");                //Ouvrir le fichier source 	
	    for(i = 0; i < 256; i++)                     //Lire les fréquences des caractères
	    {
	        fscanf(rpf, "%lld", &fre[i]);
		    blank = fgetc(rpf);                      //Espace 
	    }
	    huffmanTree(t, fre);                         //Reconstruire le Huffman arbre 
	    temp = t[510];                               //temp: le racine-point 
	    fscanf(rpf, "%lld", &size);                  //Lire la longueur du fichier  

        i = 0;
	    while(i < size)                              //Lire le contenu pour décompresser  
	    {
	        
            ch = fgetc(rpf);
		    for(j = 7; j >= 0&&i < size; j--)        //Déplacer les 8 bits du caractère àdroite, lire chaque bit, et parcourir le Huffman arbre selon les codes
		    {                                        //Parcourir jusqu'à le feuille-point, écrire le caractère généré(temp.ch) dans le fichier source 
		        ctemp = ((ch>>j)&1)+'0';
                if(ctemp == '1')
				    temp = t[temp.right];
			    else
				    temp = t[temp.left];
			    if(temp.left == -1 && temp.right == -1)  //Le feuille-point 
			    {
			        fwrite(&temp.ch, 1, 1, sf);      //Ecrire le caractère généré dans le fichier source
				    temp = t[510];
				    i++;
			    }
		    }    
	    }
	    fclose(sf);
	    
	   // cout<<"rehuffmanPack(): countt: "<<countt<<endl;
	  //  cout<<"rehuffmanPack(): name of source file: "<<sourcename<<endl;
	    if(size == 5){                                   //Tester si le contenu du fichier est "empty" 
             sf = fopen(sourcename, "rb");               //Ouvrir le fichier source 
             for(i = 0; i < size; i++){                  //lire le nom du fichier 
	             isEmpty[i] = fgetc(sf);
                 //cout<<"i: "<<i<<" isEmpty[i]: "<<isEmpty[i]<<endl; 
             }
             isEmpty[size] = '\0';
             fclose(sf);
            // cout<<"isEmpty: "<<isEmpty<<endl;
             if(!strcmp(isEmpty, "empty")){               //Si le contenu du fichier est "empty", vider ce fichier 
                 sf = fopen(sourcename, "w");
                 fclose(sf);
             }
        }
    }
}
void onlyWrite(char *filename, int size)         //Ecrire directement le contenu du fichier dans le fichier emballésans compression 
{ 
    int i=0, k, count2=0;
	unsigned char c;
	FILE *rf;

	rf = fopen(filename, "rb");                  //Ouvrir le fichier qui est déjà compressé        
    if(rf == NULL)
	{
	    cout<<"Ne peut pas ouvrir de fichier.";
		exit(0);
	}
    while(filename[count2] != '\0')              //Calculer la longueur du nom du fichier  
	    count2++;       
	fprintf(wpf, "%d", count2);                  //Ecrire la longueur du nome du fichier dans le fichier généré après emballer 
	for(i = 0; i < count2; i++)                  //Ecrire le nom du fichier dans le fichier généré après emballer 
        fprintf(wpf, "%c", filename[i]);
    fprintf(wpf, "%d", size);     
    c = fgetc(rf);
	while(!feof(rf))              //Parcourir le contenu du fichier et l'écrire dans le fichier généré après emballer
    {                            
        fprintf(wpf, "%c", c);
        c = fgetc(rf);
    }
	fclose(rf);
}
void reOnlyWrite(char *sourcename)               //Lire le contenu directement sans décompression
{
    FILE *sf;
    unsigned char ch;
    long long size, i = 0;
    
    sf = fopen(sourcename, "wb");                 
    fscanf(rpf, "%lld", &size);                  //Lire la taille du fichier
	while(i < size)                              //Ecrire le contenu directement dans le fichier source 
	{
        ch = fgetc(rpf);
        fprintf(sf, "%c", ch);
        i++;           
    }
    fclose(sf);
}
void fileCompare(void)                           //Comparer les fichiers ou répertoire
{
    int num;                                     //Le menu de la comparaison
    char fileName1[1000],fileName2[1000];        //Les noms des fichier qui ont besoin de comparer 
    FILE *cf1,*cf2;                              //Les pointeurs des fichier qui ont besoin de comparer
	struct _stat fInfo;                          //Structure du fichier qui stocke les informations du fichier
	
    cout<<"Entrez votre choix(0-2):1.Comparer deux fichiers; 2.Comparer deux répertoires; 0.Exit\n";
    cin>>num; 
    while(num != 0)
	{		
		switch(num)
		{
			case 1:				                          //Comparer deux fichiers 
				cout<<"Entrez le nom du premier fichier:\n";
			 	cin>>fileName1;
			 	cf1 = fopen(fileName1,"rb");
			 	cout<<"Entrez le nom du second fichier:\n";
			 	cin>>fileName2;
			 	cout<<"Patientez\n"; 
				cf2 = fopen(fileName2,"rb");
				if(cf1 == NULL || cf2 == NULL)
				{
                    if(cf1 == NULL)
					    cout<<"Ne peut pas ouvrir de premier fichier.\n";
				    if(cf2 == NULL)
					    cout<<"Ne peut pas ouvrir de second fichier.\n";
                }
				else
				{
					if(oneFileCompare(cf1,cf2))           //Appeler la fonction de la comparaison des fichiers  
						cout<<"Les deux fichiers sont les mêmes.\n";
					else
			 			cout<<"Les deux fichiers ne sont pas des mêmes.\n";
				}
				break;
			case 2:                                       //Comparer deux répertoires 
				cout<<"Entrez le nom du premier répertoire:\n";
			 	cin>>fileName1;
			 	cout<<"Entrez le nom du second répertoire:\n";
			 	cin>>fileName2;
			 	cout<<"Patientez\n"; 
			 	if(_stat(fileName1,&fInfo) == -1 || _stat(fileName2,&fInfo) == -1)//Obtenir les informations des fichiers, la valeur de retour:
			 	{                                                                   //-1: ne peut pas obtenir des informations de répertoire 
                    if(_stat(fileName1,&fInfo) == -1)      
	 			        cout<<"Ne peut pas ouvrir de premier répertoire.\n";                                                                         
			 	    if(_stat(fileName2,&fInfo) == -1)
 				        cout<<"Ne peut pas ouvrir de second répertoire.\n";
                }
			 	else                                 
				{
					if(fileListCheck(fileName1,fileName2) && fileListCheck(fileName2,fileName1))//Comparer les répertoires, la valeur de retour 
						cout<<"Les deux répertoires sont les mêmes.\n";                                      //1: les mêmes; 0: pas mêmes 
					else
			 			cout<<"Les deux répertoires ne sont pas des mêmes.\n";
				}
				break;
		}
		cout<<"Entrez votre choix(0-2):1.Comparer deux fichiers; 2.Comparer deux répertoires; 0.Exit\n";
		cin>>num;
	}     
}
bool oneFileCompare(FILE *cf1,FILE *cf2)    //Comparer deux fichiers, la valeur de retour: 1:les mêmes; 0: pas des mêmes
{
    long long l1, l2, ch1, ch2;
    
    fseek(cf1,0,SEEK_END);                  //Déplacer le pointeur "cf1" àla fin du fichier  
    l1=ftell(cf1);                          //Trouver la longueur du fichier, toujours utilisé avec la ligne au-dessus
    fseek(cf1,0,SEEK_SET);                  //Déplacer le pointeur "cf1" au début du fichier   
    fseek(cf2,0,SEEK_END);
    l2=ftell(cf2);
    fseek(cf2,0,SEEK_SET);
    if(l1!=l2)                              //Si les longueurs sont les mêmes 
        return false;
    for(int i=0;i<l1;i++)                   //Parcourir les deux fichiers et comparer tous les caractères                    
    {
        ch1=fgetc(cf1);
        ch2=fgetc(cf2);
        if(ch1!=ch2)
            return false;
    }
    return true;                            //Ils sont les mêmes 
}
bool fileListCheck(string path1,string path2)
{
    string temp, nPath1, nPath2;
    FILE *cf1, *cf2; 
    queue<pair<string,string> > Q;      //Récipient de la queue, son objet est le type avec deux "string" 
    pair<string, string> path;           //Un type d'objet, il est deux "string" données 
    struct _stat fInfo;                  //Le structure du fichier qui stocke les informations du fichier 
    WIN32_FIND_DATA FileData;           //Le pointeur du fichier 
    HANDLE hand;                        //La poignée  
    
    Q.push(make_pair(path1,path2));     //Obtenir une donnée de deux "string" via make_pair, après le mettre dans la queue(Q)  
    while(!Q.empty())                   //Si la queue n'est pas vide 
    {
        path = Q.front();               //Lire la donnée dans le début de la queue
        Q.pop();                        //pop la donnée 
        temp = path.first + "\\*";           //Ajouter "\\*" pour chercher 
        hand=FindFirstFile(temp.c_str(),&FileData);   //chercher le racine-fichier du répertoire，le valeur de retour est la poignée  
        if(hand != INVALID_HANDLE_VALUE)                 
        {
            while(FindNextFile(hand,&FileData) != 0)      //pas finit de parcourir les fichiers
            {
	  		    if(FileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)   //Il est un répertoire  
				{
					if(FileData.cFileName[0] == '.')                       //Ignorer le fichier "." et ".."
					    continue;
					nPath2 = path.second + "\\" + FileData.cFileName;          //Obtenir le nom de fichier prochain dans le deuxième répertoire     
					if(_stat(nPath2.c_str(),&fInfo) == -1)                 //Obtenir les informations du fichier
					    return false;
					nPath1 = path.first+"\\" + FileData.cFileName + "\\";  //Obtenir le nom de fichier prochain dans le premier répertoire  
					nPath2 += "\\";
					Q.push(make_pair(nPath1,nPath2));                      //mettre les fichiers dans la queue  
				}
				else                                                   //Il est répertoire  
				{
				    nPath1 = path.first + "\\" + FileData.cFileName;      //Obtenir le nom du permier fichier    
				    nPath2 = path.second + "\\" + FileData.cFileName;     //Obteinr le nom du deuxième fichier 
				    cf1 = fopen(nPath1.c_str(),"rb");                     //Ouvrir le premier fichier dans le objet pair  
				    cf2 = fopen(nPath2.c_str(),"rb");                     //Ouvrir le deuxième fichier dans le objet pair
				    if(cf2 == NULL)                                        
				    {
				        fclose(cf1);
				            return false;
				    }
				    if(oneFileCompare(cf1,cf2) == false)                  //Comparer les fichiers 
				        return false;
				    fclose(cf1);
				    fclose(cf2);
				}
            }
        }
    }
    return true;
}
