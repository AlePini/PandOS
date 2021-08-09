#include <umps3/umps/libumps.h>
#include <pandos_const.h>
#include <initProc.h>

#include <sysSupport.h>
#include <vmSupport.h>

#define SET_VPN(M, x) (M = (x << VPNSHIFT))
#define SET_ASID(M, x) (M = (x << ASIDSHIFT))
#define SET_D(M) (M = DIRTYON)

extern swap_t initSwapStructs();
extern void pager();
extern void generalExceptionHandler();

HIDDEN state_t UProcStructs[UPROCMAX];
HIDDEN support_t supportStructs[UPROCMAX];


void test(){

    initSwapStructs();
    initializeSemaphores();
    initializeStates();
    initializeSupports();
    instanciateUProcs();

    SYSCALL(TERMPROCESS, 0, 0, 0);


}

void instanciateUProcs(){
    for (int i = 0; i < UPROCMAX; i++)
        SYSCALL(CREATEPROCESS, &UProcStructs[i], &UProcStructs[i], 0);
}

void initializeSemaphores(){
    for (int i = 0; i < SUPP_SEM_NUMBER; i++){
        for (int j = 0; j < UPROCKMAX; j++){
            supportSemaphores[i][j] = 1;
        }
    }
}

void initializeStates(){
    for(int i=1; i<=UPROCMAX;i++){
    //Setup of status.
    UProcStructs[i]->p_s.status = IEPON | IMON | TEBITON | USERPON;
    //SP is set to RAMTOP.
    UProcStructs[i]->p_s.reg_sp = (memaddr) USERSTACKTOP;
    //Program Counter set to test address.
    UProcStructs[i]->p_s.pc_epc = (memaddr) UPROCSTARTADDR;
    UProcStructs[i]->p_s.reg_t9 = (memaddr) UPROCSTARTADDR;
    SET_ASID(UProcStructs[i]->entry_hi, id)
    }
}

void initializeSupports(){

    int status = TEBITON | IEPON | IMON;

    context_t first={ (memaddr) &sup_stackTLB[499], status, &pager};
    context_t second={ (memaddr) &sup_stackGen[499], status, &generalExceptionHandler};

    for(int=1;i<=UPROCMAX;i++){
        supportStructs[i].sup_asid=i;
        supportStructs[i]->sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = (memaddr) &(supportStructs[i]->sup_privatePgTbl[499]);
        supportStructs[i]->sup_exceptContext[GENERALEXCEPT].c_stackPtr = (memaddr) &(supportStructs[i]->sup_stackGen[499]);
        supportStructs[i]->sup_exceptContext[PGFAULTEXCEPT].c_status = status;
        supportStructs[i]->sup_exceptContext[GENERALEXCEPT].c_status = status;
        supportStructs[i]->sup_exceptContext[PGFAULTEXCEPT].c_pc = (memaddr) &pager;
        supportStructs[i]->sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) &generalExceptionHandler;

         for(int j=0;j<USERPGTBLSIZE-1;j++){

            SET_VPN(U_support_structure[i].sup_privatePgTbl[j].pte_entryHI, VPNSTART + j);
            SET_ASID(U_support_structure[i].sup_privatePgTbl[j].pte_entryHI, i);
            SET_D(U_support_structure[i].sup_privatePgTbl[j].pte_entryLO);

        }

        SET_VPN(U_support_structure[i].sup_privatePgTbl[USERPGTBLSIZE-1].pte_entryHI, UPROCSTACKSTART);
        SET_ASID(U_support_structure[i].sup_privatePgTbl[USERPGTBLSIZE-1].pte_entryHI, i);
        SET_D(U_support_structure[i].sup_privatePgTbl[USERPGTBLSIZE-1].pte_entryLO);

         }
}
