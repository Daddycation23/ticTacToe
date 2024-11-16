#include <stdio.h>
#include <stdlib.h>

// performs binary addition for two operands 
int binaryAddition(int operand_1, int operand_2) 
{
    int carry = 0;  // initialize value of carry
    int sum = 0;    // initialize value of sum
    // Iterate till there is no carry    
    while (operand_2 != 0) 
    {
        //// Please do not change the codes above ////     
        //// Please add your codes below //// 

        // calculating the carry by bitwise AND operation, then left shift 1 bit
        carry =                    ;     // please add your code in this line; same as Question (5);

        printf("\nadd carry = %d", carry);        
        // calculating the sum at each bit column, using bitwise XOR.
        sum =                     ;     // please add your code in this line; same as Question (5);

        printf("\nadd sum = %d", sum);        
        // update operand_1 value by assigning new_operand_1 = sum;
        operand_1 =               ;     // please add your code in this line; same as Question (5);

        printf("\nadd operand_1 = %d", operand_1);        
        // update operand_2 value by assigning new_operand_2 = carry
        operand_2 =               ;     // please add your code in this line; same as Question (5);

        printf("\nadd operand_2 = %d", operand_2);        
    }
    return operand_1;   // returned operand_1 value is the final sum
  }
//// Please do not change the codes below //// 

// performs binary multiplication for the given values //
int binaryMultiplication(int operand_1, int operand_2) 
{
    int product  = 0;   
    while(operand_2 != 0)   // if operand_2 is not 0, keep the computation
    {
        if (operand_2 & 0x1 == 1)    // if the MSB bit of operand_2 is 1
        {
            printf("\nmultiplier control bit is one");
            //// Please do not change the codes above ////     
            //// Please add your codes below //// 

            // add the product and operand_1, and update the value of product   
            product =                    ;     // please add your code in this line

            printf("\nproduct = %d", product);   
        }
        // shift operand_1 to left by 1 bit;
        operand_1 =                     ;     // please add your code in this line
        
        printf("\noperand_1 << 1 = [%d]", operand_1);

        // shift operand_2 to right by 1 bit;
        operand_2 =                     ;     // please add your code in this line

        printf("\noperand_2 >> 1 = [%d]", operand_2);   
    }
    return product;
  }
//// Please do not change the codes below //// 

 
// main function
int main() 
{
    int operand_1, operand_2, res;

    printf("Enter the value for operand_1 and operand_2:");
    scanf("%d %d", &operand_1, &operand_2);   

    // function call for multiplication
    res = binaryMultiplication(operand_1, operand_2);
    printf("\nBinary Multiplication result: %d\n\n", res);

  }
