#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*

Please read readme.txt for deeper understanding of how code works.
*No dependencies

Compiled using GNU GCC Compiler

This is a Complete C program which has individual functions defined for all operations

This is equivalent to a MIPS compiler which has branch not taken and stall logics

This takes an input.txt file which should be in working directory location only*********

and return an output.txt file in the same executable location.

This will tells what is current working directory

*/
/*-------------------------------Global variables----------------------------------------------*/
static int target_addr=0; // This will give PC_NUM value of where NEXT: keyword is there so starts with 0
int countStalls=0,bnezTracker[3]={0},flag_tabs=0,branched_flag=0;
static int numberOfCodeLines=1; // 1 because at EOF program will exit so adding last line by giving default value
static int stallArray[20]={0};
int print_output[1000][1000]={{0,0}}; // Output array which is used for printing O/p in particular format
static int row=0,coloumn=0;
FILE *op;

/*--------------------------------Structures definitions------------------------------------*/
typedef struct cycle_flag
{
        int IF1_flag;
        int IF2_flag;
        int ID_flag;
        int EX_flag;
        int MEM1_flag;
        int MEM2_flag;
        int MEM3_flag;
        int WB_flag;
    }cycle_flag; // To check the status of the instruction pipeline this structure is needed

typedef struct instr
{
        int operation;
        int target_operand;
        int computed_value;
        int first_operand;
        int second_operand;
    }instr; // This is similar to OPCODE which i defined for this compiler

/*------------------------------structure declarations-------------------------*/
cycle_flag test_cycle[50]={{0}};
instr instruction[50]={{0}};
instr decoded_instruction={0};


/*----------------------- All Function declarations---------------------------*/

/*-------------------------Instruction pipeline functions----------------------*/
void IF1(int program_counter[50][4],int registers[],int memorys[],int pc_num);
void IF2(int program_counter[50][4],int registers[],int memorys[],int pc_num);
void ID(int program_counter[50][4],int registers[],int memorys[],int pc_num);
void EX(int program_counter[50][4],int registers[],int memorys[],int pc_num);
void MEM1(int program_counter[50][4],int registers[],int memorys[],int pc_num);
void MEM2(int program_counter[50][4],int registers[],int memorys[],int pc_num);
void MEM3(int program_counter[50][4],int registers[],int memorys[],int pc_num);
void WB(int program_counter[50][4],int registers[],int memorys[],int pc_num);

/*-------------------------Input file reading functions------------------------*/
void register_values(int rep, int mep, int registers[],char *buffer);
void memory_values(int mep, int cep, int memorys[],char *buffer);
void code_read(int cep,int last, int program_counter[50][4], char *buffer, int pc_num);

/*--------------------------To handle stall------------------------------------*/
int ifEqual(int pc_num);

/*--------------------------To print Registers and memory values---------------*/
void print_registers(int registers[]);
void  print_memorys(int memorys[]);
void  print_PC(int program_counter[50][4]);

/*--------------------------Functions to handle output file printing-----------*/
void print_output_function();
void print_tabs(int pc_num);
void branch_tabs(int pc_num);

/*------------Support functions for register, memory and code read-------------*/
int isAlpha(char *buffer, int position);
int isNumber(char *buffer, int position);
int isR(char *buffer, int position);
int isHash(char *buffer, int position);
int isBracket(char *buffer, int position);
int cal_three_registers(int values[],int j);
int cal_three_registers1(int values[],int j);


int main(int argc, char *argv[])
{
    //local variables for main

    static char * buffer = 0;
    int i=0,k=0,exec_location=0,next_iter=1;
    char cwd[1024];
    int  pc_num=0;
    long int end_of_file=0;
    static int register_flag=0,memory_flag=0,code_flag=0, registers[32]={0}, memorys[999]={0},program_counter[500][4]={{0,0,0,0}};
    int memory_end_position=0,register_end_position=0, code_end_position=0;
    long length;
    getcwd(cwd, sizeof(cwd));
    printf("\n******Please place input.txt file in the following Current Working Directory location: \"%s\"******\n",cwd);
    printf("\nDid you have the input.txt file in the Current Working Directory?\nEnter\n 1-Yes\n 2-No");
    scanf("%d",&exec_location);

if(exec_location==1)
{
    FILE *fp = fopen("input.txt","r");
    op = fopen("output.txt","w");

// File read in to a character array for convenient processing using FILE* pointer
    if(fp)
    {
        printf("\nInput.txt opened successfully\n");
        printf("processing...\n");
        printf("\n******output.txt file is at: %s\\output.txt\n\n",cwd);
        fseek(fp,0L,SEEK_END);
        length = ftell(fp);
        fseek(fp,0,SEEK_SET);
    buffer = calloc(1,length+1);
        if(buffer)
        {
            fread(buffer,1,length,fp);
        }
    fclose(fp);
    }
    else
    {
        printf("Error in file opening, please keep input file in the same location where executable is\n");
        return 0;
    }

/*if (buffer!=NULL) // Used for debugging
{
  printf("%s\n",buffer);
}*/

//Flags to check if the file has Registers, memory and code keywords
for(i=0;i<=length;i++){
    if(buffer[i-2]=='E'&&buffer[i-1]=='R'&&buffer[i]=='S'){
        register_flag=1;
        register_end_position=i+1;
    }
    if(buffer[i-2]=='O'&&buffer[i-1]=='R'&&buffer[i]=='Y'){
        memory_flag=1;
        memory_end_position=i+2;
    }
    if(buffer[i-2]=='O'&&buffer[i-1]=='D'&&buffer[i]=='E'){
        code_flag=1;
        code_end_position=i+2;
    }
}
end_of_file=length;

        if(memory_flag==1&&register_flag==1&&code_flag==1)
        {
            register_values(register_end_position,memory_end_position,registers,buffer); //Function to read input file into registers array
            memory_values(memory_end_position,code_end_position,memorys,buffer); //Function to read input file into memorys array
            code_read(code_end_position,end_of_file,program_counter,buffer,pc_num); //Function to read input file code to Program counter array in the format of opcode

            for(pc_num=0;pc_num<numberOfCodeLines;pc_num++) // Processing all code lines
            {
                if(program_counter[pc_num][0]!=0) //No operation's opcode is zero
                {

                    if(flag_tabs>0){ // To check for any stalls and to include spaces helped for printing o/p
                       for(i=0;i<2*flag_tabs;i++)
                        {
                            //printf("\ta");
                            print_output[row][coloumn]=12;
                            coloumn++;
                        }
                    }

                    int q=pc_num;
                    if(branched_flag==0){ // To check if branching is done or not to know that is the relative position to print the next instruction
                      for(q=0;q<pc_num;q++)
                        {
                            //printf("\t");
                            print_output[row][coloumn]=12;
                            coloumn++;
                        }
                    }else
                    {
                        for(q=0;q<bnezTracker[1]+4+k;q++)
                            {
                                //printf("\td");
                                print_output[row][coloumn]=12;
                                coloumn++;
                            }
                            k++;
                    }

                    if(program_counter[pc_num][0]==9&&registers[program_counter[pc_num][1]]!=0) // 9=BNEZ operation opcode
                    {
                         // Flag is 1 if branching take place
                        IF1(program_counter,registers,memorys,pc_num);

                        if(branched_flag==1&&bnezTracker[2]<bnezTracker[1]){
                            row++;

                        }else{
                        row++;coloumn=0;
                        }
                        branched_flag=1;
                        for(q=0;q<pc_num+1;q++)
                        {
                           // printf("\t");
                            print_output[row][coloumn]=12;
                            coloumn++;
                        }
                        branch_tabs(pc_num); //Help function to print half executed instructions which are flushed due to "branch taken"
                        pc_num=program_counter[pc_num][3]-1; // To jump to instruction after successful branching
                    }
                    else
                    {
                        IF1(program_counter,registers,memorys,pc_num); // To execute instructions for failed branching and normal instructions

                    }
                    row++;
                    coloumn=0;
                }

            }

        }
        else
        {
            printf("File doesn't have one or more REGISTERS or MEMORY or CODE Keywords");
        }

print_output_function(); // Function which takes care of printing into output.txt file
print_registers(registers);  // To print final register values into output file
print_memorys(memorys); // To print final memory values into output file
fclose(fp);
fclose(op);
/*printf("Do you want to run program for another iteration? Then\n\n");
printf("Please backup output.txt file generated as re-running this program erases contents in old file\n");
printf("\nIf you still want to continue\nEnter\n 1-Yes\n 2-No\n\n");
scanf("%d",&next_iter);*/
}//End IF which checks whether the file is in exec location or not
/*else{
    printf("Please place input.txt at %s\n",cwd);
    return 0;
}*/


printf("Thank you...Have a great day\n");
return 0;
} //End main


/*----------------------------Instruction pipeline logic which has branch forwarding using instr struct's---------------------*/

void IF1(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{
    test_cycle[pc_num].IF1_flag=1;
    //printf("I%d-IF1\t",pc_num);
    print_output[row][coloumn]=1;
    coloumn++;
    IF2(program_counter,registers,memorys,pc_num);
}

void IF2(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{
    int i=0;
    if(test_cycle[pc_num].IF1_flag==1&&pc_num<numberOfCodeLines)
    {
        instruction[pc_num].operation=program_counter[pc_num][0];
        instruction[pc_num].target_operand=program_counter[pc_num][1];
        instruction[pc_num].first_operand=program_counter[pc_num][2];
        instruction[pc_num].second_operand=program_counter[pc_num][3];
        test_cycle[pc_num].IF2_flag = 1;
        for(i=0;i<20;i++){
             if(pc_num-2==stallArray[i]&&pc_num-2!=0){
                //printf("stall\tstall\t");
                print_output[row][coloumn]=99;
                coloumn++;
                print_output[row][coloumn]=99;
                coloumn++;
                flag_tabs++;
                break;
             }
        }
        //printf("I%d-IF2\t",pc_num);
        print_output[row][coloumn]=2;
        coloumn++;
        ID(program_counter,registers,memorys,pc_num);
        //printf("\n");
    }
}

void ID(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{
    int i=0;

    if(test_cycle[pc_num].IF2_flag==1)
    {
        test_cycle[pc_num].ID_flag = 1;
        for(i=0;i<20;i++){
             if(pc_num-1==stallArray[i]&&pc_num-1!=0){
                //printf("stall\tstall\t");
                print_output[row][coloumn]=99;
                coloumn++;
                print_output[row][coloumn]=99;
                coloumn++;
                break;
             }
        }

        //printf("I%d-ID\t",pc_num);
        print_output[row][coloumn]=3;
        coloumn++;
        decoded_instruction.operation=instruction[pc_num].operation;
        decoded_instruction.target_operand=instruction[pc_num].target_operand;
        switch(instruction[pc_num].operation)
        {
            case 1:
            {
                //printf("load- displacement ");
                decoded_instruction.first_operand=instruction[pc_num].first_operand;
                decoded_instruction.second_operand=registers[instruction[pc_num].second_operand];

                break;
            }
            case 2:
            {
                //printf("load- Immediate ");
                decoded_instruction.first_operand=instruction[pc_num].first_operand;
                decoded_instruction.second_operand=instruction[pc_num].second_operand;
                break;
            }

            case 3:
            {
                //printf("Store- displacement ");
                decoded_instruction.first_operand=instruction[pc_num].first_operand;
                decoded_instruction.second_operand=registers[instruction[pc_num].second_operand];

                break;
            }
            case 4:
            {
               // printf("Store- Immediate ");
                decoded_instruction.first_operand=instruction[pc_num].first_operand;
                decoded_instruction.second_operand=instruction[pc_num].second_operand;
                break;
            }
            case 5:
            {
                //printf("ADD reg ");
                decoded_instruction.first_operand=registers[instruction[pc_num].first_operand];
                decoded_instruction.second_operand=registers[instruction[pc_num].second_operand];
                break;
            }
            case 6:
            {
                //printf("ADD imm ");
                decoded_instruction.first_operand=registers[instruction[pc_num].first_operand];
                decoded_instruction.second_operand=instruction[pc_num].second_operand;
                break;
            }
            case 7:
            {
                //printf("sub reg ");
                decoded_instruction.first_operand=registers[instruction[pc_num].first_operand];
                decoded_instruction.second_operand=registers[instruction[pc_num].second_operand];
                break;
            }
            case 8:
            {
                //printf("sub imm ");
                decoded_instruction.first_operand=registers[instruction[pc_num].first_operand];
                decoded_instruction.second_operand=instruction[pc_num].second_operand;
                break;
            }
        }//end switch
    EX(program_counter,registers,memorys,pc_num);
    }//end if

}//end ID


void EX(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{
    if(test_cycle[pc_num].ID_flag==1)
    {
        test_cycle[pc_num].EX_flag=1;
        //printf("instruction[pc_num].operation:%d\n",instruction[pc_num].operation);
        int cat=ifEqual(pc_num);
        if(cat>0){
            //printf("capt:%d",cat);
            stallArray[countStalls]=pc_num;
            countStalls++;
            //printf("stall\t");
            //printf("stall\t");
            print_output[row][coloumn]=99;
            coloumn++;
            print_output[row][coloumn]=99;
            coloumn++;
        }
       // printf("I%d-EX\t",pc_num);
        print_output[row][coloumn]=4;
        coloumn++;
        switch(instruction[pc_num].operation)
        {
            case 5:
            {
                //printf("DADD- Register addr mode ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand+decoded_instruction.second_operand;
                break;
               // return instruction[pc_num].computed_value;
            }
            case 6:
            {
               // printf("DADD- IMM addr mode ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand+decoded_instruction.second_operand;
                //return instruction[pc_num].computed_value;
                break;
            }
            case 7:
            {
               // printf("sub- Register addr mode ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand-decoded_instruction.second_operand;
               // return instruction[pc_num].computed_value;
                break;
            }
            case 8:
            {
                //printf("sub- IMM addr mode ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand-decoded_instruction.second_operand;
                //return instruction[pc_num].computed_value;
                break;
            }
            case 9:
            {
                if(registers[instruction[pc_num].target_operand]!=0)
                {
                    //printf("***BNEZ ");
                    bnezTracker[0]=1; // used to check if code has bnez
                    bnezTracker[1]=pc_num; // to keep track of which line bnez triggered
                    bnezTracker[2]=target_addr; // To where it needs to be branched
                    //printf("I%d-MEM1\tI%d-MEM2\tI%d-MEM3\tI%d-WB",pc_num,pc_num,pc_num,pc_num);
                   // pc_num=target_addr-1;
                    //printf("PC num: %d ",pc_num);
                    //printf("\n");

                }
                break;
            }

        }//End Switch
        MEM1(program_counter,registers,memorys,pc_num);
       // return instruction[pc_num].computed_value;

    }//End IF

} // End EX

void MEM1(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{
    if(test_cycle[pc_num].EX_flag==1)
    {
        test_cycle[pc_num].MEM1_flag = 1;
        //printf("I%d-MEM1\t",pc_num);
        print_output[row][coloumn]=5;
        coloumn++;
        MEM2(program_counter,registers,memorys,pc_num);

    }

}
void MEM2(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{


    if(test_cycle[pc_num].MEM1_flag==1)
    {
        test_cycle[pc_num].MEM2_flag = 1;
        //printf("I%d-MEM2\t",pc_num);
        print_output[row][coloumn]=6;
        coloumn++;
        switch(instruction[pc_num].operation)
        {
            case 1:
            {
                //printf("load- displacement ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand+decoded_instruction.second_operand;
                registers[decoded_instruction.target_operand]=memorys[instruction[pc_num].computed_value];
                break;
            }
            case 2:
            {
               // printf("load- Immediate ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand+decoded_instruction.second_operand;
                registers[decoded_instruction.target_operand]=memorys[instruction[pc_num].computed_value];
                break;
            }

            case 3:
            {
               // printf("Store- displacement ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand+decoded_instruction.second_operand;
                memorys[instruction[pc_num].computed_value]=registers[decoded_instruction.target_operand];
                break;
            }
            case 4:
            {
                //printf("Store- Immediate ");
                instruction[pc_num].computed_value=decoded_instruction.first_operand+decoded_instruction.second_operand;
                memorys[instruction[pc_num].computed_value]=registers[decoded_instruction.target_operand];
                break;
            }
        }//end switch

    MEM3(program_counter,registers,memorys,pc_num);
    }//end if
}//end mem2

void MEM3(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{
    if(test_cycle[pc_num].MEM2_flag==1)
    {
        test_cycle[pc_num].MEM3_flag = 1;
        //printf("I%d-MEM3\t",pc_num);
        print_output[row][coloumn]=7;
        coloumn++;
        WB(program_counter,registers,memorys,pc_num);
    }
}

void WB(int program_counter[500][4],int registers[],int memorys[],int pc_num)
{
    if(test_cycle[pc_num].MEM3_flag==1)
    {
        test_cycle[pc_num].WB_flag = 1;
       //printf("I%d-WB\t",pc_num);
        print_output[row][coloumn]=8;
        coloumn++;
        switch(instruction[pc_num].operation)
        {
            case 5:
            {
                //printf("DADD- Register addr mode ");
                registers[instruction[pc_num].target_operand]=instruction[pc_num].computed_value;
                break;
            }
            case 6:
            {
                //printf("DADD- IMM addr mode ");
                registers[instruction[pc_num].target_operand]=instruction[pc_num].computed_value;
                break;
            }
            case 7:
            {
                //printf("sub- Register addr mode ");
                registers[instruction[pc_num].target_operand]=instruction[pc_num].computed_value;
                break;
            }
            case 8:
            {
                //printf("sub- IMM addr mode ");
                registers[instruction[pc_num].target_operand]=instruction[pc_num].computed_value;
                break;
            }
        } //END Switch

    }//End IF

}//END WB

//--------------------------------------------------------------------------------------
// ----------------------------Function to detect stall---------------------------------

int ifEqual(int pc_num){ // Function to check if stall is necessary or not
    int flag=0;
    if(pc_num>0&&instruction[pc_num-1].operation<5&&instruction[pc_num].operation>4&&instruction[pc_num].operation<9)
    {
        //printf("--kbc %d",pc_num);
        if(instruction[pc_num-1].target_operand==instruction[pc_num].target_operand)
        {
            flag=1;
        }
        if(instruction[pc_num-1].target_operand==instruction[pc_num].first_operand)
        {
            flag=2;
        }
        if(instruction[pc_num-1].target_operand==instruction[pc_num].second_operand)
        {
            flag=3;
        }

        if(instruction[pc_num-1].second_operand==instruction[pc_num].target_operand)
        {
            flag=4;
        }
        if(instruction[pc_num-1].second_operand==instruction[pc_num].first_operand)
        {
            flag=5;
        }
        if(instruction[pc_num-1].second_operand==instruction[pc_num].second_operand)
        {
            flag=6;
        }
    }else if(pc_num>0&&instruction[pc_num-1].operation>0&&instruction[pc_num-1].operation<3&&instruction[pc_num].operation<5)
    {
        if(instruction[pc_num-1].target_operand==instruction[pc_num].target_operand)
        {
            flag=7;
        }

        if(instruction[pc_num-1].target_operand==instruction[pc_num].second_operand)
        {
            flag=8;
        }
    }
    else if(pc_num>0&&instruction[pc_num].operation==9&&instruction[pc_num-1].operation>4&&instruction[pc_num-1].operation<3){
        if(instruction[pc_num-1].target_operand==instruction[pc_num].target_operand)
        {
            flag=9;
        }
    }
    return flag;
}

/*
--------------------------------------------------------------------------------------------
                              **** Register Read*****
--------------------------------------------------------------------------------------------
*/
void register_values(int rep, int mep, int registers[],char *buffer){ // Function to read input to registers
    int values[200]={0},i=0,j=0,k=0, index=0, r_flag=0;

            while((rep)<mep-5)
            {
                    if( (buffer[rep]=='R') )
                    {

                        if((buffer[rep+2]==' ')){
                           index=buffer[rep+1]-'0';
                           i=rep+1;
                        }else {
                        index=(buffer[rep+1]-'0')*10+(buffer[rep+2]-'0');
                        i=rep+2;
                        }


                        while(buffer[i]!='\n')
                        {
                           if(((buffer[i]-'0')>=0)&&((buffer[i]-'0')<=9)&&(buffer[i]!='\n'))
                            {
                               // printf("%c\n",buffer[i]);
                                values[j]=buffer[i]-'0';
                               // printf("values[%d]--%d\n",j,values[j]);
                                j++;
                            }
                            i++;
                        }
                        registers[index]=cal_three_registers(values,j);
                        //printf("index:%d,registers[index]=%d\n",index,registers[index]);
                       // break;
                    }
                rep++;
               }
            while(buffer[k]=='R')
               {
                    k=i;
                    if(buffer[k]=='R')
                    {
                        r_flag=1;
                       // printf("flag:%d\n",r_flag);
                        //break;
                    }
                    k++;
                }
                if(r_flag!=1){
                    return;
                }
                else{
                    rep=k;
                    register_values(rep,mep,registers,buffer);
                }


} //End register read
/*
--------------------------------------------------------------------------------------------
                              **** Memory Read*****
--------------------------------------------------------------------------------------------
*/
void memory_values(int mep, int cep, int memorys[],char *buffer)
{
    int values[200]={0},i=0,j=0,k=0,index=0, m_flag=0;

           while((mep)<cep-5)
               {
                    if( (buffer[mep]-'0')>=0&& (buffer[mep]-'0')<=9) //first digit encountered is MEP
                    {

                        if((buffer[mep+1]==' '))
                            {
                                index=buffer[mep]-'0';
                                i=mep+1;

                        }else if((buffer[mep+2]==' ')&&((buffer[mep+1]-'0')>=0)&&((buffer[mep+1]-'0')<=9))
                        {
                                index=(buffer[mep]-'0')*10+(buffer[mep+1]-'0');
                                i=mep+2;

                        }else if(buffer[mep+3]==' ')
                        {
                                index=(buffer[mep]-'0')*100+(buffer[mep+1]-'0')*10+(buffer[mep+2]-'0');
                                i=mep+3;

                        }

                        mep=i+1;
                        while(buffer[i]!='\n')
                        {
                           if(((buffer[i]-'0')>=0)&&((buffer[i]-'0')<=9)&&(buffer[i]!='\n'))
                            {
                                values[j]=buffer[i]-'0';
                                j++;
                            }
                            i++;
                        }
                        mep=i;
                        memorys[index]=cal_three_registers1(values,j);
                    }
                mep++;
               }
            while((buffer[k]-'0')>=0&&((buffer[k]-'0')<=9))
               {
                    k=i;
                    if((buffer[k]-'0')>=0&&((buffer[k]-'0')<=9))
                    {
                        m_flag=1;
                    }
                    k++;
               }
                if(m_flag!=1){
                    return;
                }
                else{
                    mep=k;
                    memory_values(mep,cep,memorys,buffer);
                }
} // End Memory Read

/*
--------------------------------------------------------------------------------------------
                              **** Code Read*****
--------------------------------------------------------------------------------------------
*/
void code_read(int cep,int last, int program_counter[500][4], char *buffer, int pc_num)
{
    // static int i=0;
    int j=0,immediateFlag=0,ifImmediate=0,addValue=0;
    int itterator=0,itterator1=0,countLines=0;
    //printf("\n\n--------------------------------------------------------------------------\n\n");

    if(pc_num==0) //Logic to know which line does "loop:" exist
    {
        itterator=cep;
        while(buffer[itterator]) // To place the marker at the first alphabet after "CODE" keyword
        {
            if(isAlpha(buffer,itterator))
            {
                break;
            }
            itterator++;
        }
        itterator1=itterator;
        while(buffer[itterator]) // check through the lines and when it encounters "loop:" while will be broke and will be updated to global variable
        {

            if(buffer[itterator]=='\n')
            {
                countLines++;
            }
            if(buffer[itterator]=='N'&&buffer[itterator+1]=='E'&&buffer[itterator+2]=='X'&&buffer[itterator+3]=='T'&&buffer[itterator+4]==':')
            {
                target_addr=countLines;
                break;
            }

        itterator++;
        }
        while(buffer[itterator1]) // to count number of code lines are there in input file, used  to print PC
        {
            if(buffer[itterator1]=='\n')
            {
                numberOfCodeLines++;
            }
            itterator1++;
        }
        //printf("loop: line is at %d\n",target_addr);
        //printf("Number of code lines %d\n",numberOfCodeLines);
    }

    while(buffer[cep]) //Run through line by line using recursive calls
    {
        if(buffer[cep]=='B'&&buffer[cep+1]=='N'&&buffer[cep+2]=='E'&&buffer[cep+3]=='Z')
        {
            program_counter[pc_num][0]=9;
                while(buffer[cep]!='\n'&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep))
                    {
                        program_counter[pc_num][1]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                    }
                    cep++;
                }
                program_counter[pc_num][2]=pc_num;
                program_counter[pc_num][3]=target_addr;
            for(j=0;j<4;j++)
            {
                //printf("%d\t",program_counter[pc_num][j]);
            }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);
        } //Bnez ends

        if(buffer[cep]=='D'&&buffer[cep+1]=='A'&&buffer[cep+2]=='D'&&buffer[cep+3]=='D')
        {
           // dadd((cep+5),buffer); //sending location of char after space
            //printf("DADD found\n");

            immediateFlag=0;
            ifImmediate=cep;
            while(buffer[ifImmediate]!='\n'&&buffer[ifImmediate]!='\0') //Check immediate case
            {
                if(buffer[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //if instruction is not immediate
            {
                program_counter[pc_num][0]=5;
                j=1;
                while(buffer[cep]!='\n'&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep))
                    {
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j++;
                    }
                    cep++;
                }
            while(buffer[cep]!='\n')
            {
                cep++;
            }
            for(j=0;j<4;j++){
                //printf("%d\t",program_counter[pc_num][j]);
            }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);
            }
            if(immediateFlag) //if instruction is not immediate
            {
                program_counter[pc_num][0]=6;
                j=1;
                while(buffer[cep]!='\n'&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep)||isHash(buffer,cep))
                    {
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j++;
                    }
                    cep++;
                }

            while(buffer[cep]!='\n')
            {
                cep++;
            }
            for(j=0;j<4;j++){
                //printf("%d\t",program_counter[pc_num][j]);
            }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);
            }
            break;
        } //DADD ends
        else if(buffer[cep]=='S'&&buffer[cep+1]=='U'&&buffer[cep+2]=='B')
        {
            //printf("sub found\n");
            immediateFlag=0;
            ifImmediate=cep;
            while(buffer[ifImmediate]!='\n'&&buffer[ifImmediate]!='\0') //Check immediate case
            {
                if(buffer[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //if instruction is not immediate
            {
                program_counter[pc_num][0]=7;
                j=1;
                while(buffer[cep]!='\n'&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep)){
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j++;
                    }
                    cep++;
                }
                /*program_counter[pc_num][1]= buffer[cep+6]-'0';
                program_counter[pc_num][2]= buffer[cep+10]-'0';
                program_counter[pc_num][3]= buffer[cep+14]-'0';*/

            while(buffer[cep]!='\n')
            {
                cep++;
            }
            for(j=0;j<4;j++){
                //printf("%d\t",program_counter[pc_num][j]);
            }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);
            }
            else if(immediateFlag) //if instruction is not immediate
            {
                program_counter[pc_num][0]=8;
                j=1;
                while(buffer[cep]!='\n'&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep)||isHash(buffer,cep))
                    {
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j++;
                    }
                    cep++;
                }

            while(buffer[cep]!='\n')
            {
                cep++;
            }
            for(j=0;j<4;j++){
                //printf("%d\t",program_counter[pc_num][j]);
            }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);
            }
            break;
        } //SUB ends

        else if(buffer[cep]=='S'&&buffer[cep+1]=='D')
        {
            //printf("sd found\n");
            immediateFlag=0;
            ifImmediate=cep;
            while(buffer[ifImmediate]!='\n'&&buffer[ifImmediate]!='\0') //Check immediate case
            {
                if(buffer[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //If instr not in immediate addressing mode it is in displacement mode
            {
                program_counter[pc_num][0]=3;
                j=1;
                addValue=cep+4;
                while(buffer[cep]!='\n'&&j<4&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep))
                    {
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j=j+2;
                    }
                    cep++;
                }
                while(!isBracket(buffer,addValue))
                {
                    if(buffer[addValue]==' ')
                    {
                        program_counter[pc_num][2]=isNumber(buffer,addValue+1); //passing the value after space character
                    }
                    addValue++;
                }
                while(buffer[cep]!='\n')
                {
                    cep++;
                }
                for(j=0;j<4;j++)
                {
                    //printf("%d\t",program_counter[pc_num][j]);
                }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);

            }
            else if(immediateFlag) //if instruction is immediate addressing mode
            {
                program_counter[pc_num][0]=4;
                j=1;
                while(buffer[cep]!='\n'&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep)||isHash(buffer,cep))
                    {
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j=j+2;
                    }
                    cep++;
                }

                while(buffer[cep]!='\n')
                {
                    cep++;
                }
                for(j=0;j<4;j++)
                {
                    //printf("%d\t",program_counter[pc_num][j]);
                }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);

            }
            break;

        } //SD ends

        else if(buffer[cep]=='L'&&buffer[cep+1]=='D')
        {
            //printf("LD found\n");
            immediateFlag=0;
            ifImmediate=cep;
            while(buffer[ifImmediate]!='\n'&&buffer[ifImmediate]!='\0') //Check immediate case
            {
                if(buffer[ifImmediate]=='#')
                {
                    immediateFlag=1;
                    //printf("found imm\n");

                }
              ifImmediate++;
            }
            if(!immediateFlag) //If instr not in immediate addressing mode it is in displacement mode
            {
                program_counter[pc_num][0]=1;
                j=1;
                addValue=cep+4;
                while(buffer[cep]!='\n'&&j<4&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep))
                    {
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j=j+2;
                    }
                    cep++;
                }
                while(!isBracket(buffer,addValue))
                {
                    if(buffer[addValue]==' ')
                    {
                        program_counter[pc_num][2]=isNumber(buffer,addValue+1); //passing the value after space character
                    }
                    addValue++;
                }
                while(buffer[cep]!='\n')
                {
                    cep++;
                }
                for(j=0;j<4;j++)
                {
                    //printf("%d\t",program_counter[pc_num][j]);
                }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);

            }
            else if(immediateFlag) //if instruction is immediate addressing mode
            {
                program_counter[pc_num][0]=2;
                j=1;
                while(buffer[cep]!='\n'&&buffer[cep]!='\0')
                {
                    if(isR(buffer,cep)||isHash(buffer,cep))
                    {
                        program_counter[pc_num][j]= isNumber(buffer,cep+1);
                        //printf("pc[%d][%d]=%d\n",pc_num,j,program_counter[pc_num][j]);
                        j=j+2;
                    }
                    cep++;
                }

                while(buffer[cep]!='\n')
                {
                    cep++;
                }
                for(j=0;j<4;j++)
                {
                    //printf("%d\t",program_counter[pc_num][j]);
                }
            pc_num++;
            code_read(cep+1,last,program_counter,buffer,pc_num);

            }
            break;
        } //End LD

    cep++;
    } // End Main While

} // End Code read


//-----------------------------------------------------------------------------
/*------------------------------------Printing Arrays Logic-------------------------*/

void print_registers(int registers[])
{
int i;
fprintf(op,"\n\nREGISTERS\n");
for(i=0;i<32;i++)
    {
        if(registers[i]>0)
        {
            fprintf(op,"R%d %d\n",i,registers[i]);
        }
    }
}
void  print_memorys(int memorys[])
{
int i;
fprintf(op,"\nMEMORY\n");
for(i=0;i<999;i++)
    {
        if(memorys[i]>0)
        {
            fprintf(op,"%d %d\n",i,memorys[i]);
        }
    }
}
void  print_PC(int program_counter[500][4])
{

int i=0,j=0;
//printf("\n\n-----------------------------------------------------------\n\n");
for(i=0;i<numberOfCodeLines;i++)
    {
         printf("\n");
       for(j=0;j<4;j++)
       {

           printf("%d\t",program_counter[i][j]);

       }

    }
}

//-----------------------------------------------------------------------------
//-----------------Functions used for printing---------------------------------

void print_output_function(){ // Function which takes care of printing into output.txt file
int i=0,j=0,data_flag=0;
for(j=0;j<1000;j++)
{
    data_flag=0;
    for(i=0;i<1000;i++) // TO check if data existed in a column
    {
        if(print_output[i][j]>0&&print_output[i][j]<100)
        {
            data_flag=1;
        }
    }
    if(data_flag==1)
    {
        fprintf(op,"\nC#%d ",j+1);
        for(i=0;i<1000;i++)
        {
            if(print_output[i][j]!=0)
            {
                switch(print_output[i][j])
                {
                    case 1:
                    {
                        fprintf(op,"I%d-IF1 ",i+1);
                    break;
                    }
                    case 2:
                    {
                        fprintf(op,"I%d-IF2 ",i+1);
                        break;
                    }
                    case 3:
                    {
                        fprintf(op,"I%d-ID ",i+1);
                        break;
                    }
                    case 4:
                    {
                        fprintf(op,"I%d-EX ",i+1);
                        break;
                    }
                    case 5:
                    {
                        fprintf(op,"I%d-MEM1 ",i+1);
                        break;
                    }
                    case 6:
                    {
                        fprintf(op,"I%d-MEM2 ",i+1);
                        break;
                    }
                    case 7:
                    {
                        fprintf(op,"I%d-MEM3 ",i+1);
                        break;
                    }
                    case 8:
                    {
                        fprintf(op,"I%d-WB ",i+1);
                        break;
                    }
                    case 99:
                    {
                        fprintf(op,"STALL ");
                        break;
                    }

                }//Switch end
            }//If end

            }//all row read for loop
    }
}// All column read for loop

} // end function

/*-----------------------------------------------------------------------------------------*/
/*-------------------------- Support Functions for printing--------------------------------*/

void print_tabs(int pc_num)
{
int i=0;
    for(i=0;i<pc_num+2*flag_tabs;i++)
    {
        //printf("\t");
        print_output[row][coloumn]=12;
        coloumn++;
    }
}

void branch_tabs(int pc_num)
{
    int i=0;
        if(numberOfCodeLines-pc_num-1!=0)
        {
            for(i=0;i<2*flag_tabs;i++)
            {
                //printf("\t");
                print_output[row][coloumn]=12;
                coloumn++;
            }
            //printf("I%d-IF1\tI%d-IF2\tI%d-ID\n",pc_num+1,pc_num+1,pc_num+1);
            print_output[row][coloumn]=1;
            coloumn++;
            print_output[row][coloumn]=2;
            coloumn++;
            print_output[row][coloumn]=3;
            coloumn++;
        }
        if(numberOfCodeLines-pc_num==3)
        {
            if(branched_flag==1&&bnezTracker[2]<bnezTracker[1])
                {
                    row++;
                }else{
                    row++;coloumn=0;
                }
            print_tabs(pc_num);
            //printf("\t\tI%d-IF1\tI%d-IF2\n",pc_num+2,pc_num+2,pc_num+2);
            print_output[row][coloumn]=12;
            coloumn++;
            print_output[row][coloumn]=12;
            coloumn++;
            print_output[row][coloumn]=1;
            coloumn++;
            print_output[row][coloumn]=2;
            coloumn++;
        }else if(numberOfCodeLines-pc_num>3)
        {
            if(branched_flag==1&&bnezTracker[2]<bnezTracker[1]){
                            row++;

                        }else{
                        row++;coloumn=0;
                        }
            print_tabs(pc_num);
            //printf("\t\tI%d-IF1\tI%d-IF2\n",pc_num+2,pc_num+2);
            print_output[row][coloumn]=12;
            coloumn++;
            print_output[row][coloumn]=12;
            coloumn++;
            print_output[row][coloumn]=1;
            coloumn++;
            print_output[row][coloumn]=2;
            coloumn++;
            row++;coloumn=0;
            print_tabs(pc_num);
            //printf("\t\t\tI%d-IF1\n",pc_num+3);
            print_output[row][coloumn]=12;
            coloumn++;
            print_output[row][coloumn]=12;
            coloumn++;
            print_output[row][coloumn]=12;
            coloumn++;
            print_output[row][coloumn]=1;
            coloumn++;
        }
}

/*----------------------------------------------------------------------------------------------*/
/*--------------------------**** Support Functions for code read****-----------------------------*/

int isAlpha(char *buffer, int position) //support function for code read to check if there is an alphabet
{
    int flag=0;
    if(buffer[position]>='A'&&buffer[position]<='Z')
    {
            flag=1;
    }
    return flag;

}

int isNumber(char *buffer, int position) //support function for code read to check if there is a number
{
    int number=0;

    while((buffer[position]-'0'>=0&&buffer[position]-'0'<=9))
    {
        number=number*10+(buffer[position]-'0');
        //printf("number-%d\n",number);
        position++;
    }
    position++;
return number;
}

int isR(char *buffer, int position)
{
    int flag=0;
    if(buffer[position]=='R')
    {
            flag=1;
    }
    return flag;

}

int isHash(char *buffer, int position)
{
    int flag=0;
    if(buffer[position]=='#')
    {
            flag=1;
    }
    return flag;

}

int isBracket(char *buffer, int position)
{
    int flag=0;
    if(buffer[position]=='(')
    {
            flag=1;
    }
    return flag;
}

/*------------------------------------------------------------------------------------------------*/
/*---------------------------------------- support functions for register and memory read---------*/

int cal_three_registers1(int values[],int j) // Support function for memory read
{
    int i=0,return_value=0;
    static int q=1;
    for(i=q-1;i<j;i++)
    {
        //printf("values[%d]=%d\n",i,values[i]);
        return_value=return_value*10+values[i];
    }
    q=i+1;
    //printf("Final: %d\n",return_value);
    return return_value;
}


int cal_three_registers(int values[],int j) // support function tO calc register value and send to register read function
{
    int i=0,return_value=0;
    static int q=1;
    for(i=q;i<j;i++)
    {
        //printf("values[%d]=%d\n",i,values[i]);
        return_value=return_value*10+values[i];
    }
    q=i+1;
    //printf("Final: %d\n",return_value);
    return return_value;
}
