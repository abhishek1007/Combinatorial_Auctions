#include<stdlib.h>
#include<iostream>
#include <fstream>
#include <time.h> 
#include <vector>

using namespace std;

bool** graph(int,int**,int*,int*,int);

int cur_bestprice;
int* cur_bestbids;
int cur_no_bids;

int new_bestprice;
int* new_bestbids;
int new_no_bids;

int final_bestprice;
int* final_bestbids;
int final_no_bids;

void local_search(int,int,int,int,  int*,int*,int*,int**,int*,bool**);
void random_restart(int,int,int,int,  int*,int*,int*,int**,int*,bool**);

int main(int argc, char* argv[])
{
        
        srand (time(NULL));
        
        int time;
        int coalblocks,companies,bids;
	
	ifstream file;
	ofstream output;
	
	file.open(argv[1]);
        output.open(argv[2]);
        
        // Taking inputs
	file>>time;
	file>>coalblocks;
	file>>bids;
	file>>companies;

	int b=bids;
	int c;
	int cid,cbids,n,i;
	int *l;
	int block;
	int price;
	int j;
	
	//Declaring 1-D and 2-D arrays needed
	int** bid_blocks;
	bid_blocks=new int*[bids];
	
	int * bid_price;
	bid_price=new int[bids];
	
	int* bid_num_blocks;
	bid_num_blocks=new int[bids];
	
	int* bid_cid;
	bid_cid=new int[bids];
	
	int* company_bids;
	company_bids=new int[companies];
	
	int* point;
	
	// For every bid taking input
	while(b!=0)
	{
	        file>>cid;
	        file>>cbids;
	        c=cbids;
	        
	        
	        for(j=0;j<c;j++)
	        {
	                file>>cid;
	                file>>n;
	                file>>price;

	                point=NULL;
	                point =new int[n];
	                
	                for(i=0;i<n;i++)
	                {
	                        file>>block;
	                        point[i]=block;
	                }
	                
	                bid_cid[bids-b]=cid;
	                bid_num_blocks[bids-b]=n;
	                bid_price[bids-b]=price;
	                bid_blocks[bids-b]=point;
	                b--;
	                
	        }
	        company_bids[cid-1]=cbids;
	}
	
        bool** map;
        map=graph(bids,bid_blocks,bid_num_blocks,company_bids,companies);
        
        // Initializing
        cur_bestprice=0;
        cur_bestbids=NULL;
        cur_no_bids=0;
        
        new_bestprice=0;
        new_bestbids=NULL;
        new_no_bids=0;
        
        final_bestprice=0;
        final_no_bids=0;
        final_bestbids=NULL;
        
        local_search(time, coalblocks, companies, bids,    bid_cid, bid_num_blocks, bid_price, bid_blocks, company_bids, map);

        output<<final_bestprice<<" ";        
        cout<<final_bestprice<<" ";
        
        for(int i=0;i<final_no_bids;i++)
        {
                cout<<final_bestbids[i]<<" ";
                output<<final_bestbids[i]<<" ";
        }
        
        cout<<"\n";
        output<<"\n";
        
        file.close();
        output.close();
        return 0;
        
}


// Function which creates and returns the conflict graph

bool** graph(int bids,int** bid_blocks,int* bid_num_blocks,int* company_bids,int companies) 
{

        bool** map = new bool*[bids];
        
        for(int i=0;i<bids;i++)
        {
                map[i] = new bool[bids];
        }
        
        int i,j,p,q,k,l;
        int* a;
        int* b;
        int brek=0;
        
        for(i=0;i<bids;i++)
        {
                map[i][i]=1;
                for(j=0;j<i;j++)
                {
                        map[i][j]=0;
                        a=bid_blocks[i];
                        b=bid_blocks[j];
                        p=bid_num_blocks[i];
                        q=bid_num_blocks[i];
                        
                        for(k=0;k<p;k++)
                        {
                                for(l=0;l<q;l++)
                                {
                                        if(a[k]==b[l])
                                        {
                                                brek=1;
                                                map[i][j]=1;               // conflict between bids i and j
                                                break;
                                        }
                                }
                                if(brek==1)
                                {
                                        break;
                                }
                        }
                        brek=0;        
                }
        }
                
        for(i=0;i<bids;i++)
        {
                for(j=0;j<i;j++)
                {
                        map[j][i]=map[i][j];
                }
        }        
        return map;
}


// Local search function
void local_search(int time,int coalblocks,int companies,int bids,  int* bid_cid,int* bid_num_blocks,int* bid_price,int** bid_blocks,int* company_bids,bool** map)
{
        // for passing to random restart function
        for(int j=1;j<companies;j++)
        {
                company_bids[j]=company_bids[j-1]+company_bids[j];
        }
        clock_t start,end;
        double time_taken;
        start=clock();
        while(1)
        {
        
                random_restart(time, coalblocks, companies, bids,    bid_cid, bid_num_blocks, bid_price, bid_blocks,company_bids, map);
                end=clock();
                time_taken=((double)(end-start))/CLOCKS_PER_SEC;
                
                // Storing the best price and best bis colllection
                
                if(final_bestprice<cur_bestprice)
                {
                        final_bestprice=cur_bestprice;
                        final_bestbids=cur_bestbids;
                        final_no_bids=cur_no_bids;
                }
                
                cur_bestprice=0;
                
                if(time_taken>time*60)                    // if time over, BREAK
                {
                        break;
                }
        }        
}

// Random restart function

void random_restart(int time,int coalblocks,int companies,int bids,  int* bid_cid,int* bid_num_blocks,int* bid_price,int** bid_blocks,int* company_bids, bool** map)
{
        int companies_order[companies];
        int random;
        int temp;
        
        for(int i=0;i<companies;i++)
        {
                companies_order[i]=i;
        }
        
        for(int i=0; i<companies; i++)
        {
                random=rand()%companies;
                temp=companies_order[i];
                companies_order[i]=companies_order[random];
                companies_order[random]=temp;
        }
        
        vector<int> sel_bids;
        int sel_price=0;
        int sel_company,sel_bid;
        bool bid_sel=0;
        int conflict=0;
        int k;
        new_no_bids=0;
        bool s_bids[bids];
        
        for(int i=0;i<bids;i++)
        {
                s_bids[i]=0;
        }
        
        // Selecting a random bids collection to start
        
        for(int i=0;i<companies;i++)
        {
                bid_sel=0;
                sel_company=companies_order[i];
                if(sel_company!=0)
                {
                        if(company_bids[sel_company]-company_bids[sel_company-1]!=0)
                        {
                                sel_bid=rand()%(company_bids[sel_company]-company_bids[sel_company-1]!=0);
                                sel_bid=sel_bid+company_bids[sel_company-1];
                                bid_sel=1;
                        }
                }
                else
                {
                        if(company_bids[0]!=0)
                        {
                                sel_bid=rand()%(company_bids[0]);
                                bid_sel=1;
                        }
                }
                
                if(bid_sel==1)
                {
                        conflict=0;
                        for(k=0;k<sel_bids.size();k++)
                        {
                                if(map[sel_bids[k]][sel_bid]==1)
                                {
                                        conflict=1;
                                        break;
                                }
                        }
                        if(conflict==0)
                        {
                                sel_bids.push_back(sel_bid);
                                sel_price=sel_price+bid_price[sel_bid];
                                s_bids[i]=1;
                                new_no_bids=new_no_bids+1;
                        }
                }
        }
        
        int* n_bestbids=new_bestbids;
        int new_price=0;
        
        if(n_bestbids!=cur_bestbids)
        {
                delete [] n_bestbids;
        }
        
        new_bestbids=new int[sel_bids.size()];
        new_bestprice=sel_price;
        
        for(int i=0;i<sel_bids.size();i++)
        {
                new_bestbids[i]=sel_bids[i];
        }
        
        int gap=0;
        cur_bestbids=new_bestbids;
        cur_bestprice=new_bestprice;
        cur_no_bids=new_no_bids;
        
        // Generating the neighbours and doing hill climbing
        
        for(int i=0;i<5000;i++)
        {
                new_price=0;
                new_no_bids=0;
                new_bestprice=0;
                gap=0;
                random=rand()%bids;

                if(s_bids[random]==0)
                {
                        for(int j=0;j<sel_bids.size();j++)
                        {
                                if(map[sel_bids[j]][random]==0 && bid_cid[sel_bids[j]]!=bid_cid[random])
                                {
                                        new_price=new_price+bid_price[sel_bids[j]];
                                        
                                }
                        }
                        
                        new_price=new_price+bid_price[random];
                        
                        if(new_price>=cur_bestprice)
                        
                        {
                                for(int j=0;j<sel_bids.size();j++)
                                {
                                        if(map[sel_bids[j]][random]==0 && bid_cid[sel_bids[j]]!=bid_cid[random])
                                        {
                                                sel_bids[j-gap]=sel_bids[j];
                                                new_bestprice=new_bestprice+bid_price[sel_bids[j]];
                                                new_no_bids=new_no_bids+1;
                                        }
                                        else
                                        {
                                                gap++;
                                                s_bids[j]=0;
                                        }
                                }       
                        
                                for(int j=0;j<gap;j++)
                                {
                                        sel_bids.pop_back();
                                }
                                
                                sel_bids.push_back(random);
                                new_no_bids=new_no_bids+1;
                                new_bestprice=new_bestprice+bid_price[random];
                                s_bids[random]=1;
                                n_bestbids=new_bestbids;
                                
                                if(n_bestbids!=cur_bestbids)
                                {
                                        delete [] n_bestbids;
                                }
                                
                                new_bestbids=new int[sel_bids.size()];
                                                                
                                for(int j=0;j<sel_bids.size();j++)
                                {
                                        new_bestbids[j]=sel_bids[j];
                                }
                                       
                                int gap=0;
                                
                                if(new_bestprice>cur_bestprice)
                                {
                                        cur_no_bids=new_no_bids;
                                        delete [] cur_bestbids;
                                        cur_bestbids=new_bestbids;
                                        cur_bestprice=new_bestprice;
                                }
                                
                                s_bids[random]=1;
                        }
                }                
        }        
}





