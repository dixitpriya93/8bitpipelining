#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Important: input.txt is in the same folder as the main.c program for running the program, ouput.txt too is generated at the same location
Program compiled on Command prompt using GCC compiler from Codeblocks */

static int target_addr=0;  
int no_of_stall=0,check_for_bnez[3]={0},b_flags=0,check_branching=0;
static int length=1; 
static int array_1[20]={0};
int out_display[1000][1000]={{0,0}}; // Output array 
static int rows=0,columns=0;
FILE *fopn_1;

/*reading input file*/
void reg_vals(int rep, int mep, int regs[],char *temp);
void mem_vals(int mep, int cep, int mem[],char *temp);
void rd_cd(int cep,int last, int p_cntr_prg[50][4], char *temp, int pc);

/*reading register, memory and code determined from input file*/
int b_wrd(char *temp, int pl_c);
int b_num(char *temp, int pl_c);
int b_char1(char *temp, int pl_c);
int b_char2(char *temp, int pl_c);
int b_char3(char *temp, int pl_c);
int reg_c1(int pnts[],int c);
int reg_c2(int pnts[],int c);

/*datatype declarations used for the eight stages for maintaining pipeline status and operations to be performed*/
typedef struct pip_status  // status of pipeline notified using pip_status
{
        int fetch_1;
        int fetch_2;
        int decode_1;
        int execute_1;
        int mem_1;
        int mem_2;
        int mem_3;
        int wb_1;
    }pip_status; 

typedef struct instruction_s
{
        int typ_instruction_s;	//operation performed
        int dest_val;
        int cal_val;
        int frs_src;
        int sec_src;
    }instruction_s; 

pip_status tst_val[50]={{0}};
instruction_s I_1[50]={{0}};
instruction_s decoded_instruction={0};

/* eight pipeline stages declared as functions*/
void IF1(int p_cntr_prg[50][4],int regs[],int mem[],int pc);
void IF2(int p_cntr_prg[50][4],int regs[],int mem[],int pc);
void ID(int p_cntr_prg[50][4],int regs[],int mem[],int pc);
void EX(int p_cntr_prg[50][4],int regs[],int mem[],int pc);
void MEM1(int p_cntr_prg[50][4],int regs[],int mem[],int pc);
void MEM2(int p_cntr_prg[50][4],int regs[],int mem[],int pc);
void MEM3(int p_cntr_prg[50][4],int regs[],int mem[],int pc);
void WB(int p_cntr_prg[50][4],int regs[],int mem[],int pc);

/*handling stalls*/
int eq_zr(int pc);

/*Printing output file*/
void reg_prn(int regs[]);
void  mem_prn(int mem[]);
void  prn_PC(int p_cntr_prg[50][4]);
void prnf_out();
void t_prnf(int pc);
void b_prnf(int pc);

/*printing array logic*/
void reg_prn(int regs[])
{
int a;
fprintf(fopn_1,"\n\nREGISTERS\n");
for(a=0;a<32;a++)
    {
        if(regs[a]>0)
        {
            fprintf(fopn_1,"R%d %d\n",a,regs[a]);
        }
    }
}
void  mem_prn(int mem[])
{
int a;
fprintf(fopn_1,"\nMEMORY\n");
for(a=0;a<999;a++)
    {
        if(mem[a]>0)
        {
            fprintf(fopn_1,"%d %d\n",a,mem[a]);
        }
    }
}
void  prn_PC(int p_cntr_prg[500][4])
{

int a=0,c=0;
for(a=0;a<length;a++)
    {
         printf("\n");
       for(c=0;c<4;c++)
       {

           printf("%d\t",p_cntr_prg[a][c]);

       }

    }
}

/* printing output*/

void prnf_out(){ 							// printing into output.txt file
int a=0,c=0,chk_d=0;
for(c=0;c<1000;c++)							//for loop to read in column-wise
{
    chk_d=0;
    for(a=0;a<1000;a++) 					// check if data exist in column
    {
        if(out_display[a][c]>0&&out_display[a][c]<100)
        {
            chk_d=1;
        }
    }
    if(chk_d==1)
    {
        fprintf(fopn_1,"\n c#%d ",c+1);
        for(a=0;a<1000;a++)						//for loop to read in row-wise
        {
            if(out_display[a][c]!=0)
            {
                switch(out_display[a][c])
                {
                    case 1:
                    {
                        fprintf(fopn_1," I%d-IF1 ",a+1);
                    break;
                    }
                    case 2:
                    {
                        fprintf(fopn_1," I%d-IF2 ",a+1);
                        break;
                    }
                    case 3:
                    {
                        fprintf(fopn_1," I%d-ID ",a+1);
                        break;
                    }
                    case 4:
                    {
                        fprintf(fopn_1," I%d-EX ",a+1);
                        break;
                    }
                    case 5:
                    {
                        fprintf(fopn_1," I%d-MEM1 ",a+1);
                        break;
                    }
                    case 6:
                    {
                        fprintf(fopn_1," I%d-MEM2 ",a+1);
                        break;
                    }
                    case 7:
                    {
                        fprintf(fopn_1," I%d-MEM3 ",a+1);
                        break;
                    }
                    case 8:
                    {
                        fprintf(fopn_1," I%d-WB ",a+1);
                        break;
                    }
                    case 99:
                    {
                        fprintf(fopn_1," STALL ");
                        break;
                    }

                }
            }

            }
    }
} 

}

void t_prnf(int pc)
{
int a=0;
    for(a=0;a<pc+2*b_flags;a++)
    {
        out_display[rows][columns]=12;
        columns++;
    }
}

void b_prnf(int pc)
{
    int a=0;
        if(length-pc-1!=0)
        {
            for(a=0;a<2*b_flags;a++)
            {
                out_display[rows][columns]=12;
                columns++;
            }
            out_display[rows][columns]=1;
            columns++;
            out_display[rows][columns]=2;
            columns++;
            out_display[rows][columns]=3;
            columns++;
        }
        if(length-pc==3)
        {
            if(check_branching==1&&check_for_bnez[2]<check_for_bnez[1])
                {
                    rows++;
                }else{
                    rows++;columns=0;
                }
            t_prnf(pc);
            out_display[rows][columns]=12;
            columns++;
            out_display[rows][columns]=12;
            columns++;
            out_display[rows][columns]=1;
            columns++;
            out_display[rows][columns]=2;
            columns++;
        }else if(length-pc>3)
        {
            if(check_branching==1&&check_for_bnez[2]<check_for_bnez[1]){
                            rows++;

                        }else{
                        rows++;columns=0;
                        }
            t_prnf(pc);
            out_display[rows][columns]=12;
            columns++;
            out_display[rows][columns]=12;
            columns++;
            out_display[rows][columns]=1;
            columns++;
            out_display[rows][columns]=2;
            columns++;
            rows++;columns=0;
            t_prnf(pc);
            out_display[rows][columns]=12;
            columns++;
            out_display[rows][columns]=12;
            columns++;
            out_display[rows][columns]=12;
            columns++;
            out_display[rows][columns]=1;
            columns++;
        }
}

/* Main function begins here*/
int main(int argc, char *argv[])
{
    static char * temp = 0;								//variable declarations for main function
    int a=0,b=0,filoc=0,c_nxt=1;
    char wid[1024];
    int  pc=0;
    long int file_end=0;
    static int stat_r=0,stat_m=0,stat_c=0, regs[32]={0}, mem[999]={0},p_cntr_prg[500][4]={{0,0,0,0}};
    int end_m=0,end_r=0, end_c=0;
    long length;
    getcwd(wid, sizeof(wid));
	printf(" Welcome to Adv topics project of MIPS Simulator \n \t \t Created by: priya dixit \n");
    printf("\n NOTE: Your input file and this program.c file should be in the same folder \n This program will not run if the above condition is not met \n",wid);
    printf("\nDo you adhere to the above condition?\nEnter 1 for Yes or 2 for No \n 1) Y \n 2)N \n");
    scanf("%d",&filoc);

if(filoc==1)
{
    FILE *open_f = fopen("input.txt","r");								// FILE* pointer to read file into char array
    fopn_1 = fopen("output.txt","w");				
    if(open_f)
    {
        printf("\n Thank you for meeting the requirement\n");
        printf("Your output file is on the way\n");
        printf("\n Output @ : %s\\output.txt\n\n",wid);
        fseek(open_f,0L,SEEK_END);
        length = ftell(open_f);
        fseek(open_f,0,SEEK_SET);
    temp = calloc(1,length+1);
        if(temp)
        {
            fread(temp,1,length,open_f);
        }
    fclose(open_f);
    }
    else
    {
        printf("Sorry the above conditon is not met,your input.txt file should be at %s\n",wid);
		printf("\n Do you want to continue? \n 1) Y \n 2) N");
        return 0;
    }

/* flags to decode registers,memory and code from input file*/
for(a=0;a<=length;a++){
    if(temp[a-2]=='E'&&temp[a-1]=='R'&&temp[a]=='S'){
        stat_r=1;
        end_r=a+1;
    }
    if(temp[a-2]=='O'&&temp[a-1]=='R'&&temp[a]=='Y'){
        stat_m=1;
        end_m=a+2;
    }
    if(temp[a-2]=='O'&&temp[a-1]=='D'&&temp[a]=='E'){
        stat_c=1;
        end_c=a+2;
    }
}
file_end=length;

        if(stat_m==1&&stat_r==1&&stat_c==1)
        {
            reg_vals(end_r,end_m,regs,temp); 					//reading input to (registers) array regs_val
            mem_vals(end_m,end_c,mem,temp);							//reading input to (memory) array mem_vals
            rd_cd(end_c,file_end,p_cntr_prg,temp,pc); 				//reading input to program counter array (read code)

            for(pc=0;pc<length;pc++) 
				{
                if(p_cntr_prg[pc][0]!=0) // if typ_instruction_s's opcode is not zero
                {

                    if(b_flags>0){ // stalls are detected if any and whitespaces are included in output.txt
                       for(a=0;a<2*b_flags;a++)
                        {
                            //printf("\ta");
                            out_display[rows][columns]=12;
                            columns++;
                        }
                    }

                    int num_1=pc;
                    if(check_branching==0){ 			// check for branching or for the relative position to print the instruction
                      for(num_1=0;num_1<pc;num_1++)
                        {
                            //printf("\t");
                            out_display[rows][columns]=12;
                            columns++;
                        }
                    }else
                    {
                        for(num_1=0;num_1<check_for_bnez[1]+4+b;num_1++)
                            {
                                //printf("\td");
                                out_display[rows][columns]=12;
                                columns++;
                            }
                            b++;
                    }

                    if(p_cntr_prg[pc][0]==9&&regs[p_cntr_prg[pc][1]]!=0) // opcode for branch not equal to zero = 9
                    {
                         // Flag is 1 if branching take place
                        IF1(p_cntr_prg,regs,mem,pc);

                        if(check_branching==1&&check_for_bnez[2]<check_for_bnez[1]){
                            rows++;

                        }else{
                        rows++;columns=0;
                        }
                        check_branching=1;
                        for(num_1=0;num_1<pc+1;num_1++)
                        {
                           // printf("\t");
                            out_display[rows][columns]=12;
                            columns++;
                        }
                        b_prnf(pc); 					//printing partially executed instructions flushed due to BRANCH TAKEN
                        pc=p_cntr_prg[pc][3]-1; 		// go to the target address of the branch instruction
                    }
                    else
                    {
                        IF1(p_cntr_prg,regs,mem,pc); 		// To execute instructions for failed branching and normal instructions

                    }
                    rows++;
                    columns=0;
                }

            }

        }
        
	prnf_out(); 							// printing to output.txt file
	reg_prn(regs);  						// printing resultant final register values to output file
	mem_prn(mem); 							// printing resultant final memory values to output file
	fclose(open_f);
	fclose(fopn_1);
}
printf("program execution completed, find your output file at above location");
return 0;
}

/*read code*/
void rd_cd(int cep,int last, int p_cntr_prg[500][4], char *temp, int pc)
{
    
    int c=0,immediateFlag=0,ifImmediate=0,addValue=0;
    int itterator=0,itterator1=0,countLines=0;					//static int a=0

    if(pc==0) 													//Logic to know which line does "loop:" exist
    {
        itterator=cep;
        while(temp[itterator]) 								// To place the marker at the first alphabet after "CODE" keyword
        {
            if(b_wrd(temp,itterator))
            {
                break;
            }
            itterator++;
        }
        itterator1=itterator;
        while(temp[itterator]) // check through the lines and when it encounters "loop:" while will be broke and will be updated to global variable
        {

            if(temp[itterator]=='\n')
            {
                countLines++;
            }
            if(temp[itterator]=='N'&&temp[itterator+1]=='E'&&temp[itterator+2]=='X'&&temp[itterator+3]=='T'&&temp[itterator+4]==':')
            {
                target_addr=countLines;
                break;
            }

        itterator++;
        }
        while(temp[itterator1]) // to count number of code lines are there in input file, used  to print PC
        {
            if(temp[itterator1]=='\n')
            {
                length++;
            }
            itterator1++;
        }
        //printf("loop: line is at %d\n",target_addr);
        //printf("Number of code lines %d\n",length);
    }

    while(temp[cep]) //Run through line by line using recursive calls
    {
        if(temp[cep]=='B'&&temp[cep+1]=='N'&&temp[cep+2]=='E'&&temp[cep+3]=='Z')
        {
            p_cntr_prg[pc][0]=9;
                while(temp[cep]!='\n'&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep))
                    {
                        p_cntr_prg[pc][1]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                    }
                    cep++;
                }
                p_cntr_prg[pc][2]=pc;
                p_cntr_prg[pc][3]=target_addr;
            for(c=0;c<4;c++)
            {
                //printf("%d\t",p_cntr_prg[pc][c]);
            }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);
        } //Bnez ends

        if(temp[cep]=='D'&&temp[cep+1]=='A'&&temp[cep+2]=='D'&&temp[cep+3]=='D')
        {
           // dadd((cep+5),temp); //sending location of char after space
            //printf("DADD found\n");

            immediateFlag=0;
            ifImmediate=cep;
            while(temp[ifImmediate]!='\n'&&temp[ifImmediate]!='\0') //Check immediate case
            {
                if(temp[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //if instruction is not immediate
            {
                p_cntr_prg[pc][0]=5;
                c=1;
                while(temp[cep]!='\n'&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep))
                    {
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                        c++;
                    }
                    cep++;
                }
            while(temp[cep]!='\n')
            {
                cep++;
            }
            for(c=0;c<4;c++){
                //printf("%d\t",p_cntr_prg[pc][c]);
            }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);
            }
            if(immediateFlag) //if instruction is not immediate
            {
                p_cntr_prg[pc][0]=6;
                c=1;
                while(temp[cep]!='\n'&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep)||b_char2(temp,cep))
                    {
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                        c++;
                    }
                    cep++;
                }

            while(temp[cep]!='\n')
            {
                cep++;
            }
            for(c=0;c<4;c++){
                //printf("%d\t",p_cntr_prg[pc][c]);
            }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);
            }
            break;
        } //DADD ends
        else if(temp[cep]=='S'&&temp[cep+1]=='U'&&temp[cep+2]=='B')
        {
            //printf("sub found\n");
            immediateFlag=0;
            ifImmediate=cep;
            while(temp[ifImmediate]!='\n'&&temp[ifImmediate]!='\0') //Check immediate case
            {
                if(temp[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //if instruction is not immediate
            {
                p_cntr_prg[pc][0]=7;
                c=1;
                while(temp[cep]!='\n'&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep)){
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                        c++;
                    }
                    cep++;
                }
                /*p_cntr_prg[pc][1]= temp[cep+6]-'0';
                p_cntr_prg[pc][2]= temp[cep+10]-'0';
                p_cntr_prg[pc][3]= temp[cep+14]-'0';*/

            while(temp[cep]!='\n')
            {
                cep++;
            }
            for(c=0;c<4;c++){
                //printf("%d\t",p_cntr_prg[pc][c]);
            }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);
            }
            else if(immediateFlag) //if instruction is not immediate
            {
                p_cntr_prg[pc][0]=8;
                c=1;
                while(temp[cep]!='\n'&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep)||b_char2(temp,cep))
                    {
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                        c++;
                    }
                    cep++;
                }

            while(temp[cep]!='\n')
            {
                cep++;
            }
            for(c=0;c<4;c++){
                //printf("%d\t",p_cntr_prg[pc][c]);
            }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);
            }
            break;
        } 

        else if(temp[cep]=='S'&&temp[cep+1]=='D')
        {
            //printf("sd found\n");
            immediateFlag=0;
            ifImmediate=cep;
            while(temp[ifImmediate]!='\n'&&temp[ifImmediate]!='\0') //Check immediate case
            {
                if(temp[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //If instruction_s not in immediate addressing mode it is in displacement mode
            {
                p_cntr_prg[pc][0]=3;
                c=1;
                addValue=cep+4;
                while(temp[cep]!='\n'&&c<4&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep))
                    {
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                        c=c+2;
                    }
                    cep++;
                }
                while(!b_char3(temp,addValue))
                {
                    if(temp[addValue]==' ')
                    {
                        p_cntr_prg[pc][2]=b_num(temp,addValue+1); //passing the value after space character
                    }
                    addValue++;
                }
                while(temp[cep]!='\n')
                {
                    cep++;
                }
                for(c=0;c<4;c++)
                {
                    //printf("%d\t",p_cntr_prg[pc][c]);
                }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);

            }
            else if(immediateFlag) //if instruction is immediate addressing mode
            {
                p_cntr_prg[pc][0]=4;
                c=1;
                while(temp[cep]!='\n'&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep)||b_char2(temp,cep))
                    {
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                        c=c+2;
                    }
                    cep++;
                }

                while(temp[cep]!='\n')
                {
                    cep++;
                }
                for(c=0;c<4;c++)
                {
                    //printf("%d\t",p_cntr_prg[pc][c]);
                }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);

            }
            break;

        } //SD ends

        else if(temp[cep]=='L'&&temp[cep+1]=='D')
        {
            //printf("LD found\n");
            immediateFlag=0;
            ifImmediate=cep;
            while(temp[ifImmediate]!='\n'&&temp[ifImmediate]!='\0') //Check immediate case
            {
                if(temp[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //If instruction_s not in immediate addressing mode it is in displacement mode
            {
                p_cntr_prg[pc][0]=1;
                c=1;
                addValue=cep+4;
                while(temp[cep]!='\n'&&c<4&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep))
                    {
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc,c,p_cntr_prg[pc][c]);
                        c=c+2;
                    }
                    cep++;
                }
                while(!b_char3(temp,addValue))
                {
                    if(temp[addValue]==' ')
                    {
                        p_cntr_prg[pc][2]=b_num(temp,addValue+1); //passing the value after space character
                    }
                    addValue++;
                }
                while(temp[cep]!='\n')
                {
                    cep++;
                }
                for(c=0;c<4;c++)
                {
                    //printf("%d\t",p_cntr_prg[pc][c]);
                }
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);

            }
            else if(immediateFlag) //if instruction is immediate addressing mode
            {
                p_cntr_prg[pc][0]=2;
                c=1;
                while(temp[cep]!='\n'&&temp[cep]!='\0')
                {
                    if(b_char1(temp,cep)||b_char2(temp,cep))
                    {
                        p_cntr_prg[pc][c]= b_num(temp,cep+1);
                        
                        c=c+2;
                    }
                    cep++;
                }

                while(temp[cep]!='\n')
                {
                    cep++;
                }
                
            pc++;
            rd_cd(cep+1,last,p_cntr_prg,temp,pc);

            }
            break;
        } 

    cep++;
    } 

} 

/*memory read*/
void mem_vals(int mep, int cep, int mem[],char *temp)
{
    int pnts[200]={0},a=0,c=0,b=0,index=0, m_flag=0;

           while((mep)<cep-5)
               {
                    if( (temp[mep]-'0')>=0&& (temp[mep]-'0')<=9) //first digit encountered is MEP
                    {

                        if((temp[mep+1]==' '))
                            {
                                index=temp[mep]-'0';
                                a=mep+1;

                        }else if((temp[mep+2]==' ')&&((temp[mep+1]-'0')>=0)&&((temp[mep+1]-'0')<=9))
                        {
                                index=(temp[mep]-'0')*10+(temp[mep+1]-'0');
                                a=mep+2;

                        }else if(temp[mep+3]==' ')
                        {
                                index=(temp[mep]-'0')*100+(temp[mep+1]-'0')*10+(temp[mep+2]-'0');
                                a=mep+3;

                        }

                        mep=a+1;
                        while(temp[a]!='\n')
                        {
                           if(((temp[a]-'0')>=0)&&((temp[a]-'0')<=9)&&(temp[a]!='\n'))
                            {
                                pnts[c]=temp[a]-'0';
                                c++;
                            }
                            a++;
                        }
                        mep=a;
                        mem[index]=reg_c2(pnts,c);
                    }
                mep++;
               }
            while((temp[b]-'0')>=0&&((temp[b]-'0')<=9))
               {
                    b=a;
                    if((temp[b]-'0')>=0&&((temp[b]-'0')<=9))
                    {
                        m_flag=1;
                    }
                    b++;
               }
                if(m_flag!=1){
                    return;
                }
                else{
                    mep=b;
                    mem_vals(mep,cep,mem,temp);
                }
} 

/*register read*/
void reg_vals(int rep, int mep, int regs[],char *temp){ 				// Function to read input file to regs array
    int pnts[200]={0},a=0,c=0,b=0, index=0, r_flag=0;

            while((rep)<mep-5)
            {
                    if( (temp[rep]=='R') )
                    {

                        if((temp[rep+2]==' ')){
                           index=temp[rep+1]-'0';
                           a=rep+1;
                        }else {
                        index=(temp[rep+1]-'0')*10+(temp[rep+2]-'0');
                        a=rep+2;
                        }


                        while(temp[a]!='\n')
                        {
                           if(((temp[a]-'0')>=0)&&((temp[a]-'0')<=9)&&(temp[a]!='\n'))
                            {
                               // printf("%c\n",temp[a]);
                                pnts[c]=temp[a]-'0';
                               // printf("values[%d]--%d\n",c,values[c]);
                                c++;
                            }
                            a++;
                        }
                        regs[index]=reg_c1(pnts,c);
                        //printf("index:%d,regs[index]=%d\n",index,regs[index]);
                       // break;
                    }
                rep++;
               }
            while(temp[b]=='R')
               {
                    b=a;
                    if(temp[b]=='R')
                    {
                        r_flag=1;
                       // printf("flag:%d\n",r_flag);
                        //break;
                    }
                    b++;
                }
                if(r_flag!=1){
                    return;
                }
                else{
                    rep=b;
                    reg_vals(rep,mep,regs,temp);
                }


}

/*supporting register read*/

int reg_c2(int pnts[],int c) 					// read memory values
{
    int a=0,rtn_n=0;
    static int num_1=1;
    for(a=num_1-1;a<c;a++)
    {
        rtn_n=rtn_n*10+pnts[a];
    }
    num_1=a+1;
    return rtn_n;
}


int reg_c1(int pnts[],int c) 					// calculate and read register values
{
    int a=0,rtn_n=0;
    static int num_1=1;
    for(a=num_1;a<c;a++)
    {
        rtn_n=rtn_n*10+pnts[a];
    }
    num_1=a+1;
    return rtn_n;
}
/*supporting read code*/

int b_wrd(char *temp, int pl_c) 				//function for code read to check if there is an alphabet
{
    int mrkr=0;
    if(temp[pl_c]>='A'&&temp[pl_c]<='Z')
    {
            mrkr=1;
    }
    return mrkr;

}

int b_num(char *temp, int pl_c) 				//function for code read to check if there is a number
{
    int vls=0;

    while((temp[pl_c]-'0'>=0&&temp[pl_c]-'0'<=9))
    {
        vls=vls*10+(temp[pl_c]-'0');
        //printf("vls-%d\n",vls);
        pl_c++;
    }
    pl_c++;
return vls;
}

int b_char1(char *temp, int pl_c)
{
    int mrkr=0;
    if(temp[pl_c]=='R')
    {
            mrkr=1;
    }
    return mrkr;

}

int b_char2(char *temp, int pl_c)
{
    int mrkr=0;
    if(temp[pl_c]=='#')
    {
            mrkr=1;
    }
    return mrkr;

}

int b_char3(char *temp, int pl_c)
{
    int mrkr=0;
    if(temp[pl_c]=='(')
    {
            mrkr=1;
    }
    return mrkr;
}

/*each of the 8 pipeline stage functions*/

void IF1(int p_cntr_prg[500][4],int regs[],int mem[],int pc)				//function for fetching cycle 1 stage of a pipeline 
{
    tst_val[pc].fetch_1=1;
    out_display[rows][columns]=1;
    columns++;
    IF2(p_cntr_prg,regs,mem,pc);
}

void IF2(int p_cntr_prg[500][4],int regs[],int mem[],int pc)				//function for fetching cycle 2 stage of a pipeline 
{
    int a=0;
    if(tst_val[pc].fetch_1==1&&pc<length)
    {
        I_1[pc].typ_instruction_s=p_cntr_prg[pc][0];
        I_1[pc].dest_val=p_cntr_prg[pc][1];
        I_1[pc].frs_src=p_cntr_prg[pc][2];
        I_1[pc].sec_src=p_cntr_prg[pc][3];
        tst_val[pc].fetch_2 = 1;
        for(a=0;a<20;a++){
             if(pc-2==array_1[a]&&pc-2!=0){
                out_display[rows][columns]=99;
                columns++;
                out_display[rows][columns]=99;
                columns++;
                b_flags++;
                break;
             }
        }
        out_display[rows][columns]=2;
        columns++;
        ID(p_cntr_prg,regs,mem,pc);
        
    }
}

void ID(int p_cntr_prg[500][4],int regs[],int mem[],int pc)				//function for decoding stage of pipeline
{
    int a=0;

    if(tst_val[pc].fetch_2==1)
    {
        tst_val[pc].decode_1 = 1;
        for(a=0;a<20;a++){
             if(pc-1==array_1[a]&&pc-1!=0){
                out_display[rows][columns]=99;
                columns++;
                out_display[rows][columns]=99;
                columns++;
                break;
             }
        }
		out_display[rows][columns]=3;
        columns++;
        decoded_instruction.typ_instruction_s=I_1[pc].typ_instruction_s;
        decoded_instruction.dest_val=I_1[pc].dest_val;
        switch(I_1[pc].typ_instruction_s)
        {
            case 1:
            {
                decoded_instruction.frs_src=I_1[pc].frs_src;
                decoded_instruction.sec_src=regs[I_1[pc].sec_src];

                break;
            }
            case 2:
            {
                decoded_instruction.frs_src=I_1[pc].frs_src;
                decoded_instruction.sec_src=I_1[pc].sec_src;
                break;
            }

            case 3:
            {
                decoded_instruction.frs_src=I_1[pc].frs_src;
                decoded_instruction.sec_src=regs[I_1[pc].sec_src];

                break;
            }
            case 4:
            {
               decoded_instruction.frs_src=I_1[pc].frs_src;
               decoded_instruction.sec_src=I_1[pc].sec_src;
                break;
            }
            case 5:
            {
                decoded_instruction.frs_src=regs[I_1[pc].frs_src];
                decoded_instruction.sec_src=regs[I_1[pc].sec_src];
                break;
            }
            case 6:
            {
                decoded_instruction.frs_src=regs[I_1[pc].frs_src];
                decoded_instruction.sec_src=I_1[pc].sec_src;
                break;
            }
            case 7:
            {
                decoded_instruction.frs_src=regs[I_1[pc].frs_src];
                decoded_instruction.sec_src=regs[I_1[pc].sec_src];
                break;
            }
            case 8:
            {
                decoded_instruction.frs_src=regs[I_1[pc].frs_src];
                decoded_instruction.sec_src=I_1[pc].sec_src;
                break;
            }
        }
    EX(p_cntr_prg,regs,mem,pc);
    }
}

void EX(int p_cntr_prg[500][4],int regs[],int mem[],int pc)				// function for execution stage of pipeline
{
    if(tst_val[pc].decode_1==1)
    {
        tst_val[pc].execute_1=1;
        int rnt=eq_zr(pc);
        if(rnt>0){
            array_1[no_of_stall]=pc;
            no_of_stall++;
            out_display[rows][columns]=99;
            columns++;
            out_display[rows][columns]=99;
            columns++;
        }
       out_display[rows][columns]=4;
        columns++;
        switch(I_1[pc].typ_instruction_s)
        {
            case 5:
            {
                I_1[pc].cal_val=decoded_instruction.frs_src+decoded_instruction.sec_src;
                break;
            }
            case 6:
            {
               I_1[pc].cal_val=decoded_instruction.frs_src+decoded_instruction.sec_src;
               break;
            }
            case 7:
            {
               I_1[pc].cal_val=decoded_instruction.frs_src-decoded_instruction.sec_src;
               break;
            }
            case 8:
            {
                I_1[pc].cal_val=decoded_instruction.frs_src-decoded_instruction.sec_src;
                break;
            }
            case 9:
            {
                if(regs[I_1[pc].dest_val]!=0)
                {
                    check_for_bnez[0]=1; 					// If code has BNEZ, bnez[0]=1
                    check_for_bnez[1]=pc; 					// program counter address at which branch has started
                    check_for_bnez[2]=target_addr; 			// target address of the branch instruction
                    
                }
                break;
            }

        }
        MEM1(p_cntr_prg,regs,mem,pc);
        }

}

void MEM1(int p_cntr_prg[500][4],int regs[],int mem[],int pc)				//function for memory cycle 1 stage of the pipeline
{
    if(tst_val[pc].execute_1==1)
    {
        tst_val[pc].mem_1 = 1;
        out_display[rows][columns]=5;
        columns++;
        MEM2(p_cntr_prg,regs,mem,pc);

    }

}
void MEM2(int p_cntr_prg[500][4],int regs[],int mem[],int pc)				//function for memory cycle 2 stage of the pipeline
{


    if(tst_val[pc].mem_1==1)
    {
        tst_val[pc].mem_2 = 1;
        out_display[rows][columns]=6;
        columns++;
        switch(I_1[pc].typ_instruction_s)
        {
            case 1:
            {
                I_1[pc].cal_val=decoded_instruction.frs_src+decoded_instruction.sec_src;
                regs[decoded_instruction.dest_val]=mem[I_1[pc].cal_val];
                break;
            }
            case 2:
            {
               I_1[pc].cal_val=decoded_instruction.frs_src+decoded_instruction.sec_src;
                regs[decoded_instruction.dest_val]=mem[I_1[pc].cal_val];
                break;
            }

            case 3:
            {
               I_1[pc].cal_val=decoded_instruction.frs_src+decoded_instruction.sec_src;
                mem[I_1[pc].cal_val]=regs[decoded_instruction.dest_val];
                break;
            }
            case 4:
            {
                I_1[pc].cal_val=decoded_instruction.frs_src+decoded_instruction.sec_src;
                mem[I_1[pc].cal_val]=regs[decoded_instruction.dest_val];
                break;
            }
        }

    MEM3(p_cntr_prg,regs,mem,pc);
    }
}

void MEM3(int p_cntr_prg[500][4],int regs[],int mem[],int pc)					//function for memory cycle 3 stage of the pipeline
{
    if(tst_val[pc].mem_2==1)
    {
        tst_val[pc].mem_3 = 1;
		out_display[rows][columns]=7;
        columns++;
        WB(p_cntr_prg,regs,mem,pc);
    }
}

void WB(int p_cntr_prg[500][4],int regs[],int mem[],int pc)					//function for write back stage of pipeline
{
    if(tst_val[pc].mem_3==1)
    {
        tst_val[pc].wb_1 = 1;
        out_display[rows][columns]=8;
        columns++;
        switch(I_1[pc].typ_instruction_s)
        {
            case 5:
            {
                regs[I_1[pc].dest_val]=I_1[pc].cal_val;
                break;
            }
            case 6:
            {
                regs[I_1[pc].dest_val]=I_1[pc].cal_val;
                break;
            }
            case 7:
            {
                regs[I_1[pc].dest_val]=I_1[pc].cal_val;
                break;
            }
            case 8:
            {
                regs[I_1[pc].dest_val]=I_1[pc].cal_val;
                break;
            }
        }

    }

}

/* detecting stalls */

int eq_zr(int pc){ 
    int mrkr=0;
    if(pc>0&&I_1[pc-1].typ_instruction_s<5&&I_1[pc].typ_instruction_s>4&&I_1[pc].typ_instruction_s<9)		// Checking if stall is required
    {
        //printf("--kbc %d",pc);
        if(I_1[pc-1].dest_val==I_1[pc].dest_val)
        {
            mrkr=1;
        }
        if(I_1[pc-1].dest_val==I_1[pc].frs_src)
        {
            mrkr=2;
        }
        if(I_1[pc-1].dest_val==I_1[pc].sec_src)
        {
            mrkr=3;
        }

        if(I_1[pc-1].sec_src==I_1[pc].dest_val)
        {
            mrkr=4;
        }
        if(I_1[pc-1].sec_src==I_1[pc].frs_src)
        {
            mrkr=5;
        }
        if(I_1[pc-1].sec_src==I_1[pc].sec_src)
        {
            mrkr=6;
        }
    }else if(pc>0&&I_1[pc-1].typ_instruction_s>0&&I_1[pc-1].typ_instruction_s<3&&I_1[pc].typ_instruction_s<5)
    {
        if(I_1[pc-1].dest_val==I_1[pc].dest_val)
        {
            mrkr=7;
        }

        if(I_1[pc-1].dest_val==I_1[pc].sec_src)
        {
            mrkr=8;
        }
    }
    else if(pc>0&&I_1[pc].typ_instruction_s==9&&I_1[pc-1].typ_instruction_s>4&&I_1[pc-1].typ_instruction_s<3){
        if(I_1[pc-1].dest_val==I_1[pc].dest_val)
        {
            mrkr=9;
        }
    }
    return mrkr;
}



