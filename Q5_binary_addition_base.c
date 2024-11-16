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
        carry =                   ;     // please add your code in this line
        // print out the carry value
        printf("For add operation: carry = %d\n", carry);
        // calculating the sum at each bit column, using bitwise XOR.
        sum =                     ;     // please add your code in this line
                
        printf("For add operation: sum = %d\n", sum);
        // update operand_1 value by assigning new_operand_1 = sum;
        operand_1 =               ;     // please add your code in this line
              
        printf("For add operation: operand_1 = %d\n", operand_1);
        // update operand_2 value by assigning new_operand_2 = carry
        operand_2 =               ;     // please add your code in this line
        
        printf("For add operation: operand_2 = %d\n", operand_2);
    }
    return operand_1;   // returned operand_1 value is the final sum
  }

  //// Please do not change the codes below ////   

// performs binary subtraction for operand_1 - operand_2 
int binarySubtracton(int operand_1, int operand_2) 
{
    int carry = 0;  // initialize value of carry
    int sum = 0;    // initialize value of sum

    // operand_1 - operand_2 = operand_1 + (-operand_2)
    // Step 1 for (-operand_2): finding two's complement for operand_2 = ~operand_2 + 1
    operand_2 = binaryAddition(~operand_2, 1);    

    // Step 2 for operand_1 + (-operand_2): perform addition of two binary operands 
    // Iterate till there is no carry
    while (operand_2 != 0) 
    {
        //// Please do not change the codes above ////                     
        //// Please add your codes below //// 

        // calculating the carry by bitwise AND operation, then left shift 1 bit
        carry =                 ;     // please add your code in this line

        printf("For subtraction operation: carry = %d\n", carry);
        // calculating the sum at each bit column, using bitwise XOR.
        sum =                   ;     // please add your code in this line

        printf("For subtraction operation: sum = %d\n", sum);
        // update operand_1 value by assigning new_operand_1 = sum;
        operand_1 =              ;     // please add your code in this line

        printf("For subtraction operation: operand_1 = %d\n", operand_1);
        // update operand_2 value by assigning new_operand_2 = carry
        operand_2 =              ;     // please add your code in this line

        printf("For subtraction operation: operand_2 = %d\n", operand_2);
    }
    return operand_1;
  }
//// Please do not change the codes below //// 


// main function
int main() 
{
    int operand_1, operand_2, res;

    printf("Enter the value for operand_1 and operand_2:");
    scanf("%d %d", &operand_1, &operand_2);   

    // function call for addition of two binary numbers
    res = binaryAddition(operand_1, operand_2);
    printf("Binary Addition result: %d\n\n", res);

    // function call for subtraction of two binary numbers
    res = binarySubtracton(operand_1, operand_2);
    printf("Binary Subtracton result: %d\n\n", res);

  }
