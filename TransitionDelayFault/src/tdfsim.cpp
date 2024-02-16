/**********************************************************************/
/*  Parallel-Fault Event-Driven Transition Delay Fault Simulator      */
/*                                                                    */
/*           Author: Tsai-Chieh Chen                                  */
/*           last update : 10/22/2018                                 */
/**********************************************************************/

#include "atpg.h"

/* pack 16 faults into one packet.  simulate 16 faults together. 
 * the following variable name is somewhat misleading */
#define num_of_pattern 16

/* The faulty_wire contains a list of wires that 
 * change their values in the fault simulation for a particular packet.
 * (that is wire_value1 != wire_value2) 
 * Note that the wire themselves are not necessarily a fault site.
 * The list is linked by the pnext pointers */

/* fault simulate a set of test vectors */
//void ATPG::transition_delay_fault_simulation(int &total_detect_num) {

//}

/*
void ATPG::generate_tdfault_list() {
    
}
*/

///////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
void ATPG::generate_tdfault_list()
{
  
  wptr wp;
  
  
  
  
  
  fptr_s fp;
  
  
  
  int fn;
  nptr ko;

  auto st = sort_wlist.crbegin();
  auto ed = sort_wlist.crend();

  
  for (auto p = st;p!=ed; ++p)
  {
    wp= *p;
    ko = wp->inode.front();


    fp = move(fptr_s(new(nothrow) FAULT));


    //if (fp==nullptr) error("error");
    
    fp->node = ko;
    fp->io = GO;
    fp->fault_type = STR;
    fp->to_swlist = wp->wlist_index;
    fp->detected_time = 0;
    
    
    

    /////////////////////////////////////////////////////////
    switch (ko->type)
    {
      case NOT:
      case BUF:
        fp->eqv_fault_num = 1;
        for (wptr wpr: wp->inode.front()->iwire)
        {
          if (wpr->onode.size()>1)
          {
            fp->eqv_fault_num++;
          } 


        }
        break;
      case AND:
      case NOR:
      case INPUT:
      case OR:
      case NAND:
      case EQV:
      case XOR:
        fp->eqv_fault_num = 1;
        break;
    }


    num_of_gate_fault += fp->eqv_fault_num;
    flist_undetect.push_front(fp.get());
    flist.push_front(move(fp));
    

    
    fp = move(fptr_s(new(nothrow) FAULT));
    //if (fp == nullptr) error("No more room!");


    
    fp->node = ko;
    fp->io = GO;
    fp->fault_type = STF;
    fp->to_swlist = wp->wlist_index;
    fp->detected_time = 0;
    
    
    
    ////////////////////////////////////////////////////
    switch (ko->type)
    {

      case NOT:
      case BUF:
        fp->eqv_fault_num = 1;
        for (wptr wpr: wp->inode.front()->iwire)
        {
          if (wpr->onode.size()>1)
          {
            fp->eqv_fault_num++;
          } 
        }
        break;
      case OR:
      case NAND:
      case INPUT:
      case AND:
      case NOR:
      case EQV:
      case XOR:
        fp->eqv_fault_num = 1;
        break;
    }
    ///////////////////////////////////////////////////////////
    num_of_gate_fault += fp->eqv_fault_num;
    flist_undetect.push_front(fp.get());
    flist.push_front(move(fp));
    
    

    ///////////////////////////////////////////////////////////////
    if(wp->onode.size()>1)
    {
      for (nptr npr: wp->onode)
      {
        /////////////////////////////////////////////
        switch (npr->type) 
        {
          case OUTPUT:
          case OR:
          case NOR:
          case AND:
          case NAND:
          case EQV:
          case XOR:
            fp = move(fptr_s(new(nothrow) FAULT));
            //if (fp == nullptr) error("error");
            
            fp->node = npr;
            fp->io = GI;
            fp->fault_type = STR;
            fp->to_swlist = wp->wlist_index;
            fp->eqv_fault_num = 1;
            fp->detected_time = 0;
            
            
            
            
          
            for (int m=0;m<npr->iwire.size(); m++)
            {
              if (npr->iwire[m] == wp)
              {
                fp->index = m;
              } 
            }

            num_of_gate_fault++;
            flist_undetect.push_front(fp.get());
            flist.push_front(move(fp));
            
            break;
        }
        ///////////////////////////////////////////////////////////
        switch (npr->type)
        {
          case OUTPUT:
          case OR:
          case NOR:
          case AND:
          case NAND:
          case EQV:
          case XOR:
            fp = move(fptr_s(new(nothrow) FAULT));
            //if (fp == nullptr) error("error!");
            
            fp->node = npr;
            fp->io = GI;

            fp->fault_type = STF;

            fp->to_swlist = wp->wlist_index;
            fp->eqv_fault_num = 1;
            
            
            fp->detected_time = 0;
            
            


            for (int m=0;m<npr->iwire.size(); m++)
            {
              if (npr->iwire[m] == wp)
              {
                fp->index = m;
              } 
            }
            num_of_gate_fault++;
            flist_undetect.push_front(fp.get());
            flist.push_front(move(fp));
            
            break;
        }

      }
    }

  }

  num_of_tdf_fault = 0;
  fn=0;
  
  for (fptr fm: flist_undetect)
  {
    fm->fault_no = fn;
    fn++;
    num_of_tdf_fault += fm->eqv_fault_num;

    
  }


}



//////////////////////////////////////////////////////////////////
















void ATPG::tdfault_sim_a_vector(const string &vv, int &curr)
{

  
  bool con1=false;
  bool con2=false;
  fptr df;

  for (int i=0;i<cktin.size();i++)
  {
    cktin[i]->value=ctoi(vv[i]);
  }

  for (int j = 0; j <sort_wlist.size();j++)
  {

    if(j<cktin.size())
    {
      con1=true;
    }else
    {
      con1=false;
    }



    if(!con1)
    {
      sort_wlist[j]->value = U;
    }else
    {
      sort_wlist[j]->set_changed();
    }

  }

  sim();
  

  for (auto p=flist_undetect.cbegin();p!=flist_undetect.cend();++p)
  {
    df=*p;

    int sw=df->to_swlist;


    if(sort_wlist[sw]->value==df->fault_type)
    {
      
      con2=true;
    }else
    {
      con2=false;
    }





    if(!con2)
    {
      df->activate=FALSE;
    }else
    {
      df->activate=TRUE;
    }

  }

  tdfault_sim_a_vector2(vv,curr);

}
////////////////////////////////////////////////////////////////////












void ATPG::tdfault_sim_a_vector2(const string &vv, int &nc)
{
  //////////////////////////////////////////////////////
  for (int i=0;i<cktin.size();i++)
  {
    if(i!=0)
    {
      cktin[i]->value=ctoi(vv[i-1]);
    }
    else
    {
      cktin[i]->value=ctoi(vv[cktin.size()]);
    }
  }
  ///////////////////////////////////////////////////////

  for (int j=0;j<sort_wlist.size();j++)
  {
    if (j<cktin.size())
    {
      sort_wlist[j]->set_changed();
    }else
    {
      sort_wlist[j]->value = U;
    }
  }
  /////////////////////////////////////////////////////

  sim();
  //////////////////////////////////////
  if(debug)
  { 
    display_io(); 
  }
  //////////////////////////////////////////

  
  for(int i = 0;i<sort_wlist.size(); i++)
  {

    if(sort_wlist[i]->value==0)
    {
      sort_wlist[i]->wire_value2 = ALL_ZERO;
      sort_wlist[i]->wire_value1 = ALL_ZERO; 
    }

    if(sort_wlist[i]->value==1)
    {
      sort_wlist[i]->wire_value2 = ALL_ONE;
      sort_wlist[i]->wire_value1 = ALL_ONE;  
    }

    if(sort_wlist[i]->value==2)
    {
      sort_wlist[i]->wire_value1 = 0x55555555; 
      sort_wlist[i]->wire_value2 = 0x55555555;
    }
  } 
  /////////////////////////////////////////////////////

  wptr fw;
  int nf=0; 
  
  fptr sfl[num_of_pattern];
  int ft;
  int swi=9999;
  fptr fp;
  wptr wpp;


  bool check1=false;
  bool check2=false;
  bool check3=false;

  

  ///////////////////////////////////////////////////////////////////////////////////
  for (auto p= flist_undetect.cbegin(); p!= flist_undetect.cend(); ++p)
  {
    int fdt[num_of_pattern] = {0};
    
    
    
    fp = *p;
    if (fp->detect==REDUNDANT)
    {   
      continue; 
    } 
    
    if (fp->activate == FALSE)
    {

      if(next(p, 1)==flist_undetect.cend())
      {
        check1=true;
      }else
      {
        check1=false;
      }

      if(nf> 0)
      {
        check2=true;
      }else
      {
        check2=false;
      }




      if(check1 && check2)
      {
        

        for (int i=swi;i<sort_wlist.size(); i++)
        {
          if (sort_wlist[i]->is_scheduled())
          {
            sort_wlist[i]->remove_scheduled();
            fault_sim_evaluate(sort_wlist[i]);
          }
        } 

        ///////////////////////////////////////////////////////////////
        while (!wlist_faulty.empty())
        {
          wpp = wlist_faulty.front();
          wlist_faulty.pop_front();
          wpp->remove_faulty();
          wpp->remove_fault_injected();
          wpp->set_fault_free();
        
          if (wpp->is_output())
          { 

            for (int i=0;i<nf; i++)
            { 
              if (!(sfl[i]->detect))
              {
                if ((wpp->wire_value2 & Mask[i])^(wpp->wire_value1 & Mask[i]))
                {


                  if((wpp->wire_value2 & Mask[i]) ^ Unknown[i])
                  {
                    check1=true;
                  }else
                  {
                    check1=false;
                  }

                  if((wpp->wire_value1 & Mask[i]) ^ Unknown[i])
                  {
                    check2=true;
                  }else
                  {
                    check2=false;
                  }




                  if(check1 && check2)
                  {
                    fdt[i] = 1;
                  }



                }
                /////////////////////////////
              }
            }

          }
          wpp->wire_value2 = wpp->wire_value1; 
        
        }
        ////////////////////////////////////////////////////////
        for (int i = 0;i<nf; i++)
        {
          if (fdt[i] == 1)
          {
            sfl[i]->detect = TRUE;
          }

        }
        nf= 0;  
        swi=9999;  






















      }
      
    }else
    { 
      /////////////////////////////////////////////////////////////////////////////////////
      int cc=fp->to_swlist;
    
      if (fp->fault_type != sort_wlist[cc]->value) 
      {

        if(fp->node->type == OUTPUT)
        {
          check1=true;
        }else
        {
          check1=false;
        }

        if(fp->io==GO)
        {
          check2=true;
        }else
        {
          check2=false;
        }

        if(sort_wlist[cc]->is_output())
        {
          check3=true;
        }else
        {
          check3=false;
        }





        if((check1)||(check2 && check3))
        {
          fp->detect = TRUE;
        }else
        {

          if (check2)
          {

         
              if (!(sort_wlist[cc]->is_faulty()))
              {
                sort_wlist[cc]->set_faulty();
                wlist_faulty.push_front(sort_wlist[cc]);
              }

          
              sfl[nf] = fp;
              inject_fault_value(sort_wlist[cc],nf, fp->fault_type);

         
              sort_wlist[cc]->set_fault_injected();


              auto st = sort_wlist[cc]->onode.cbegin();
              auto ed = sort_wlist[cc]->onode.cend();


              for (auto k = st; k != ed; ++k)
              {
                (*k)->owire.front()->set_scheduled();
              }

         
              nf++;
         
              swi = min(swi, fp->to_swlist);
          }else
          {

         
            fw = get_faulty_wire(fp, ft);
            if(fw!= nullptr) 
            {

           
              if (fw->is_output())
              {
                fp->detect=TRUE;
              }else 
              {
              
                if (!(fw->is_faulty()))
                {
                  fw->set_faulty();
                  wlist_faulty.push_front(fw);
                }

             
                sfl[nf]=fp;
                inject_fault_value(fw, nf, ft);

              
                fw->set_fault_injected();


                auto st = fw->onode.cbegin();
                auto ed = fw->onode.cend();


                for (auto k = st; k != ed; ++k)
                {
                  (*k)->owire.front()->set_scheduled();
                }

                nf++;
                swi=min(swi, cc);
              }
            }
          //////////////////////////////////////////////////////////
          }
          /////////////////////////////////////
        }
      }
      /////////////////////////////////////////////////////////////////////


      if(nf== num_of_pattern)
      {
        check1=true;
      }else
      {
        check1=false;
      }

      if(next(p, 1) == flist_undetect.cend())
      {
        check2=true;
      }else
      {
        check2=false;
      }



    

      if (check1 || check2)
      {
        
      
        for (int i=swi;i<sort_wlist.size(); i++)
        {
          if (sort_wlist[i]->is_scheduled())
          {
            sort_wlist[i]->remove_scheduled();
            fault_sim_evaluate(sort_wlist[i]);
          }
        } 

        ///////////////////////////////////////////////////////////////
        while (!wlist_faulty.empty())
        {
          wpp = wlist_faulty.front();
          wlist_faulty.pop_front();
          wpp->remove_faulty();
          wpp->remove_fault_injected();
          wpp->set_fault_free();
        
          if (wpp->is_output())
          { 

            for (int i=0;i<nf; i++)
            { 
              if (!(sfl[i]->detect))
              {
                if ((wpp->wire_value2 & Mask[i])^(wpp->wire_value1 & Mask[i]))
                {


                  if((wpp->wire_value2 & Mask[i]) ^ Unknown[i])
                  {
                    check1=true;
                  }else
                  {
                    check1=false;
                  }

                  if((wpp->wire_value1 & Mask[i]) ^ Unknown[i])
                  {
                    check2=true;
                  }else
                  {
                    check2=false;
                  }



                  if(check1 && check2)
                  {
                    fdt[i] = 1;
                  }



                }
                /////////////////////////////
              }
            }

          }
          wpp->wire_value2 = wpp->wire_value1; 
        
        }
      ////////////////////////////////////////////////////////
        for (int i = 0;i<nf; i++)
        {
          if (fdt[i] == 1)
          {
            sfl[i]->detect = TRUE;
          }

        }
        nf= 0;  
        swi=9999;  
      }
      ///////////////////////////////////////////////////// 
    } 
  }
  //////////////////////////////////////////////////////////////
  nc= 0;

  
  flist_undetect.remove_if(
      [&](const fptr see) {
        if (see->detect==TRUE)
        {
          string IO;
         
          nc += see->eqv_fault_num;

          return true;
        }else
        {
          return false;
        }
      });

}
/////////////////////////////////////////////////////////////////



void ATPG::transition_delay_fault_simulation(int &total)
{

  int curr=0;

  for (int i =vectors.size()-1;i>=0;i--)
  {
    tdfault_sim_a_vector(vectors[i],curr);
    total+=curr;
    fprintf(stdout, "vector[%d] detects %d faults (%d)\n", i, curr, total);
  }

}
////////////////////////////////////////////////////////////






