/**
 * @file template.h
 * @brief SOME file for IFJ Project 2017 AT vut.fit.vutbr.cz
 * @date xx.xx.xxxx, last rev. xx.xx.xxxx
 * @author Name Surname - login
 * @author Name Surname - login
 *
 * Module description: Description of the modul's purpose and responsibilities.
 */

#ifndef MODULE_NAME_H
#define MODULE_NAME_H 1

/*		Includes section	*/
#include <limits.h>

/*	Macros	section		*/
/** @brief Macro definition 	*/
#define ONE 1

/*	Constants	*/
const char *exampleMsg = "Zapocet: nie";

/*	Data types	*/
/**
 * @struct exStruct
 * @brief Brief description of what this structure represents
 * @var poll_waiter::mem1 member1 description
 * @var poll_waiter::mem2 member2 description
 */
struct exStruct;

/**
 * @enum myEnum
 * @brief Brief description of what this enum represents
 */
enum myEnum
{
	val1		///< Comment of variable 1,
	val2		///< Comment of variable 2,
};

/*	Prototypes	*/
/**
 * @brief Brief description
 *
 * Description of this function's purpose and results.
 *
 * @param[in] exPar Description of this input parameter
 * @param[out] *exPar2 Description of this output parameter.
 * @return Description of return values of this function.
 */
void exampleFunction(int exPar, char *exPar2);

/*	Functions definitions (inline)	*/

#endif // MODULE_NAME_H
