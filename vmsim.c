/*
 * vmsim.c: Virtual memory implementation skeleton code
 * with a single level 32-Bit page table and
 * fifo page replacement algorithm
 * ToDO: Implement other requested page replacement algorithms
 *       
 * (c) Mohammad Hasanzadeh Mofrad, 2019
 * (e) moh18@pitt.edu
 *
 */
 
#include "vmsim.h"


unsigned int hashCode(unsigned int key, int size) {
   return key % size;
}

struct DataItem *search(struct DataItem** hashArray, unsigned int key, int size) {
   //get the hash 
   unsigned int hashIndex = hashCode(key, size);  
	
   //move in array until an empty 
   while(hashArray[hashIndex] != NULL) {
	
      if(hashArray[hashIndex]->key == key)
         return hashArray[hashIndex]; 
			
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= size;
   }        
	
   return NULL;        
}

void insert(struct DataItem** hashArray, unsigned int key,int data, int size) {

   
   int found = 0;
   //get the hash 
   unsigned int hashIndex = hashCode(key, size); 
   //move in array until an empty or deleted cell
   while(hashArray[hashIndex] != NULL) {
   	  if(hashArray[hashIndex]->key == key)
   	  {
   	  	found = 1;
   	  	break;
   	  }
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= size;
   }
   if(found == 0)
   { 
   	 struct DataItem *item = (struct DataItem*) malloc(sizeof(struct DataItem));
   	 item->key = key;
   	 struct datalis* head = (struct datalis*) malloc(sizeof(struct datalis));
     item->head = head;
     item->head->value = data;
     item->tail = head;
     head->next = NULL; 
     hashArray[hashIndex] = item;
    
     

   }
   else{
      struct datalis * new = (struct datalis*) malloc(sizeof(struct datalis));
      hashArray[hashIndex]->tail->next = new;
      hashArray[hashIndex]->tail = hashArray[hashIndex]->tail->next;
      hashArray[hashIndex]->tail->value = data;
      hashArray[hashIndex]->tail->next = NULL;
      
   }

	
}

void delete(struct DataItem** hashArray, unsigned int k, int size) {
   unsigned int key = k;

   //get the hash 
   unsigned int hashIndex = hashCode(key, size);
   
   //move in array until an empty
   while(1) {
   	//printf("inside loop\n");
   	  if(hashArray[hashIndex] != NULL)
   	  {
   	  	//printf("hashIndex is present\n");
   	  	 if(hashArray[hashIndex]->key == key) 
   	  	{
   	  		
      	//if there is more than 1 values in the value list
      	if(hashArray[hashIndex]->head->next != NULL)
      	{
      		
      		struct datalis* temp = hashArray[hashIndex]->head->next;
      		free(hashArray[hashIndex]->head);
      		hashArray[hashIndex]->head = temp;
      		temp = NULL;
      		
      	}
      	//if its the only value
      	else
      	{
      		
           
      		hashArray[hashIndex]->tail = NULL;
      		free(hashArray[hashIndex]->head);
      		hashArray[hashIndex] = NULL; 
      		
      	}
      	break;
   	  }
	 
      }
		
      //go to next cell
      ++hashIndex;
		
      //wrap around the table
      hashIndex %= size;
   }      
	
       
}
void printbinary (uint8_t n)
{
	int k;
	int c;
  for (c = 7; c >= 0; c--)
  {
    k = n >> c;
 
    if (k & 1)
      printf("1");
    else
      printf("0");
  }
 
}

void shift(struct frame_struct* frames, int refresh)
{
	struct frame_struct* curr = frames;
	while(curr)
	{ 
		//if frame has a page 
		if(curr->pte_pointer)
		{ 
			
			//push in reference bit
			uint8_t r = page_table[PTE32_INDEX(curr->virtual_address)]->reference_counter;
			
			
			//shift right by 1 and set the msb to 1 if refrence bit is 1
            
            r = r >> 1;
            if (page_table[PTE32_INDEX(curr->virtual_address)]->reference_bit == 1)
            {
            	r = r | (uint8_t)128;
            	
            }
            r = r >> (refresh - 1);
			//reset reference bit
			page_table[PTE32_INDEX(curr->virtual_address)]->reference_bit= 0;
			page_table[PTE32_INDEX(curr->virtual_address)]->reference_counter = (uint8_t)r;
			

		}
		curr=curr->next;
	}

}


void* allocate(unsigned long int size) {
    void* ptr = NULL;
    if(size) {        
        ptr = malloc(size);
        if(!ptr) {
            fprintf(stderr, "Error on mallocing memory\n");
            exit(1);
        }
        memset(ptr, 0, size);
    }
    return(ptr);
}
/*void* deallocate(void** ptr, unsigned long int size)
{

	if(size)
	{
		free(ptr);
	}
}*/
int main(int argc, char* argv[]) {
	
   /*
    * Add sanity check for input arguments
    * Open the memory trace file 
    * and store it in an array
    */
    
    /*if(argc != 6) {
        fprintf(stderr, "USAGE: %s -n <numframes> -a <fifo> <tracefile>\n", argv[0]);
        exit(1);         
    }*/
     
    
    numframes = atoi(argv[2]);
    char* algorithm= argv[4];
    char* filename;
    int refresh = 0;
    if(!strcmp(argv[5],"-r"))
    {
    	refresh = atoi(argv[6]);
    	filename = argv[7];
    }
    else
    {
    	filename = argv[5];
    }
    FILE* file = fopen(filename,"rb");
    if(!file) {
        fprintf(stderr, "Error on opening %s\n", filename);
        exit(1); 
    }
     
    /* 
     * Calculate the trace file's length
     * and read in the trace file
     * and write it into addr_arr and mode_arr arrays 
     */

    unsigned int numaccesses = 0;
    unsigned char mode = '\0';
    unsigned int addr = 0;
    unsigned int cycles = 0;

    // Calculate number of lines in the trace file
    while(fscanf(file, "%c %x %d\n", &mode, &addr, &cycles) == 3) {
        numaccesses++;
    }
    rewind(file);

    unsigned char mode_array[numaccesses];
    unsigned int address_array[numaccesses];
    unsigned int cycles_array[numaccesses];
    
    unsigned int i = 0;
    // Store the memory accesses 
    while(fscanf(file, "%c %x %d\n", &mode_array[i], &address_array[i], &cycles_array[i]) == 3) {
        i++;
    }

    if(fclose(file)) {
        fprintf(stderr, "Error on closing %s\n", filename);
        exit(1);
    }
    
    if(i != numaccesses) {
        fprintf(stderr, "Arrays are populated incorrectly\n");
        exit(1);
    }

    // Initialize the physical memory address space
    long frame_size = PAGE_SIZE_4KB;
    long memory_size = frame_size * numframes;
    physical_frames = (unsigned int*) allocate(memory_size);
    
    // Create the first frame of the frames linked list
    struct frame_struct* head = NULL;

    struct frame_struct* curr = NULL;
    for(i = 0; i < numframes; i++) {
        if(i == 0) {
            head = (struct frame_struct*) allocate(sizeof(struct frame_struct));
            curr = head;
        }
        else {
            curr->next = (struct frame_struct*) allocate(sizeof(struct frame_struct));
            curr = curr->next;
        }
        curr->frame_number = i;
        curr->physical_address = physical_frames + (i * frame_size);
        curr->virtual_address = 0;
        curr->pte_pointer     = NULL;
        //curr->dirty = (unsigned char)0;
    }

    // Initialize page table
    long page_table_size = PT_SIZE_1MB * PTE_SIZE_BYTES;
    page_table = (struct pte_32**) allocate(page_table_size * sizeof(struct pte_32*));

    
    struct pte_32* new_pte = NULL;
    unsigned char mode_type = '\0';
    unsigned int fault_address = 0;
    int hit = 0;
    int page2evict = 0;
    int numfaults = 0;
    int numdirty = 0;
    int last_refresh = 0;
    int sum_cycles = 0;


     
    //if algorithm is opt, process memory accesses to populate the hashtable
    struct DataItem** hashArray = allocate(numaccesses * sizeof(struct DataItem*));
    int k;
    	for(k=0; k < numaccesses; k++)
        {
    	    hashArray[k] = NULL;
        }
    
    
    if(!strcmp(algorithm, "opt")){
    	

        int j;
        for(j = 0; j < numaccesses; j++) {

        	//insert all addresses in hashtable
        	
        	insert(hashArray, PTE32_INDEX(address_array[j]), j, numaccesses);
        }
    }

   
    
    // Main loop to process memory accesses
    for(i = 0; i < numaccesses; i++) {
        fault_address = address_array[i];
        mode_type = mode_array[i];
        hit = 0;
        // Perform page walk for the fault address
        new_pte = (struct pte_32*) handle_page_fault(fault_address); //pte of the fault address

        if(mode_type == 's') {
            new_pte->dirty = 1;
        }

        if(!strcmp(algorithm, "opt"))
        {
        //delete the page key and time from hashtable
        delete(hashArray, PTE32_INDEX(address_array[i]), numaccesses);
        //update the lru counter
        ((struct pte_32*) page_table[PTE32_INDEX(fault_address)])->lru_counter = i;
         
         }

        else if(!strcmp(algorithm, "aging"))
        {

         	
         	//calculate total cycles elapsed
         	sum_cycles+=cycles_array[i];
         	if(i > 0) sum_cycles+=1;
            int diff = sum_cycles - last_refresh;

         	//check if diff >= refresh
         	if(diff >= refresh)
         	{
         		//calculate number of refreshes
         		int num_refreshes = diff/refresh;
         		//update last refresh
         		last_refresh = last_refresh + (num_refreshes*refresh);
         		
         		shift(head, num_refreshes);
            }

            
            

       }
       
      
        /*
         * Traverse the frames linked list    
         * to see if the requested page is present in
         * the frames linked list.
         */
        curr = head;
        while(curr) {
        	
        	//do the base addresses match
            if(curr->physical_address == new_pte->physical_address) {

            	//is page present in memory
                if(new_pte->present) {
                    curr->virtual_address = fault_address;
                    hit = 1;
                    
                    //its a hit so reset reference bit to 1 
            	    new_pte->reference_bit = 1;
            	    
                    #ifdef DEBUG
                    printf("%5d: page hit   – no eviction %010u (0x%08x)\n", i, (unsigned int) ((uintptr_t) fault_address), fault_address);
                    printf("%5d: page hit   - keep page   %010u (0x%08x) accessed (0x%08x)\n", i, (unsigned int) ((uintptr_t) curr->physical_address), *((unsigned int*) &new_pte->physical_address), curr->virtual_address);
                    #endif
                    break;

                }
                
                
            }
            
                curr = curr->next;
            
        }

        /* 
         * If the requested page is not present in the
         * frames linked list use page replacement
         * to evict the victim frame and swap in the requested frame
         */  
       if(!hit) {
            // Fifo page replacement algorithm
            if(!strcmp(algorithm, "fifo")) {
            	
                page2evict = fifo();
              
            }
            else if (!strcmp(algorithm, "opt"))
            {
            	
            	page2evict = opt(address_array, numaccesses, hashArray, head);
            }
            else if(!strcmp(algorithm, "aging"))
            {
            	//its not a hit so reset reference bit to 0 and reference counter to 10000000 
            	new_pte->reference_counter = (uint8_t)128;
            	new_pte->reference_bit = 0;
            	page2evict = age(head);
            	
            }
            /* Traverse the frames linked list to
            * find the victim frame and swap it out
            * Set the present bit and collect some statistics
            * ToDO: Need to add your dirty bit implementation 
            * inside the while loop
            */
            curr = head;
            while(curr) {
                if(curr->frame_number == page2evict) {
                    
                    numfaults++;

                    if(curr->pte_pointer) {
                        curr->pte_pointer->present = 0;
                        if(curr->pte_pointer->dirty) {
                            numdirty++;
                            curr->pte_pointer->dirty = 0;
                        }
                        
                    }
                    
                    curr->pte_pointer = (struct pte_32*) new_pte;
                    new_pte->physical_address = curr->physical_address;
                    new_pte->present = 1;
                    curr->virtual_address = fault_address; 
                 
                    

                    break;
                
                }
                curr = curr->next; 
            }
            
            


           
        }
        
    }
    
    /* ToDo: Release the memory you allocated for frames and page_table */
    //free hash array
    
    free(hashArray);
    free(page_table);
    
    //free frames:
    int h;
    struct frame_struct* temp = head;
    for(h = 0; h <numframes - 1; h++)
    {
    	head = head->next;
    	free(temp);
    	temp = head;
    }
    free(head);
    free(physical_frames);
    
    
    
    
    if (!strcmp(algorithm, "fifo")) printf("Algorithm: %s\n", "FIFO");
    else if (!strcmp(algorithm, "opt")) printf("Algorithm: %s\n", "OPT");
    else if (!strcmp(algorithm, "aging")) printf("Algorithm: %s\n", "AGING");
    printf("Number of frames: %d\n", numframes);
    printf("Total memory accesses: %d\n", i);
    printf("Total page faults: %d\n", numfaults);
    printf("Total writes to disk: %d\n", numdirty);

    
    
    //print total writes to disk based on dirty bit implementation
    
    return(0);
}

struct frame_struct * handle_page_fault(unsigned int fault_address) {
    pte = (struct pte_32*) page_table[PTE32_INDEX(fault_address)];
    //if no memory address on that page table entry(page table entry is empty)
    if(!pte) {
        pte = allocate(sizeof(struct pte_32));
        pte->present = 0;
        pte->physical_address = NULL;
        pte->lru_counter= 0;
        page_table[PTE32_INDEX(fault_address)] = (struct pte_32*) pte;
    }

    #ifdef DEBUG
    printf("############## Page fault handler ##############\n");    
    printf("Fault address:           %010u (0x%08x)\n", (unsigned int) ((uintptr_t) fault_address), fault_address);
    printf("Page table base address: %010u (0x%08x)\n", (unsigned int) ((uintptr_t) page_table), *((unsigned int*) &page_table));
    printf("PTE offset:              %010u (0x%08x)\n", PTE32_INDEX(fault_address), PTE32_INDEX(fault_address));
    printf("PTE index:               %010u (0x%08x)\n", (unsigned int) ((uintptr_t)(PTE32_INDEX(fault_address) * PTE_SIZE_BYTES)), (unsigned int) ((uintptr_t)(PTE32_INDEX(fault_address) * PTE_SIZE_BYTES)));
    printf("PTE virtual address:     %010u (0x%08x)\n", (unsigned int) ((uintptr_t) page_table + PTE32_INDEX(fault_address)), *((unsigned int*) &*page_table + PTE32_INDEX(fault_address)));
    printf("Physcial base address:   %010u (0x%08x)\n", (unsigned int) ((uintptr_t) pte->physical_address), *((unsigned int*) &pte->physical_address));
    printf("Frame offset:            %010u (0x%08x)\n", FRAME_INDEX(fault_address), FRAME_INDEX(fault_address));
    printf("Frame index:             %010u (0x%08x)\n", FRAME_INDEX(fault_address) * PAGE_SIZE_UNITS, FRAME_INDEX(fault_address) * PAGE_SIZE_UNITS);
    printf("Frame physical address:  %010u (0x%08x)\n", (unsigned int) ((uintptr_t) pte->physical_address + FRAME_INDEX(fault_address)), *((unsigned int*) &pte->physical_address + FRAME_INDEX(fault_address)));
    #endif
    
    return ((struct frame_struct *) pte);
}

//fifo implementation
int fifo() {
	int res= current_index++;
	res = res % numframes;
    return res;
}

//opt implementation

int opt(unsigned int* address_array, int numaccesses, struct DataItem** hashArray, struct frame_struct* frames) {

	//loop through frames to find a null pte reference

    struct frame_struct* curr = frames;
    while(curr)
    {  // if found send that frames frame number
    	if(curr->pte_pointer == NULL)
    	{
    		
    		return curr->frame_number;
    	}
    	curr= curr->next;
    }
    //if (current_index < numframes-1) return fifo();

    curr = frames;
    //else loop through frames to find the page from hashtable which has the highest value greater than i
    int furthest_time = -1;
    int frame_n = 0;
    int lru = numaccesses;
    int never_accessed = 0;

    while(curr)
    {
    	//if the value corresponding to the virtual page is greater than furthest_time, set furthest_address equal to that page
    	struct DataItem* page = search(hashArray, PTE32_INDEX(curr->virtual_address), numaccesses);
    	//if page is in hashtable, get its time, otherwise it will never be accessed, set never accessed and look for teis using LRU counter
    	if(page == NULL)
    	{
    		never_accessed = 1;
    		//return curr->frame_number;
    		struct pte_32* newpte = (struct pte_32*)page_table[PTE32_INDEX(curr->virtual_address)];
    		if (newpte->lru_counter < lru)
    		{
    			lru = newpte->lru_counter;
	        	frame_n = curr->frame_number;
    		}
    	}

    	else if(page != NULL)
    	{
    		if(never_accessed == 0)
    		{
    			int time = page->head->value;
		        if(time > furthest_time)
		        {
		        	furthest_time = time;
		        	frame_n = curr->frame_number;

		        }
    		}
    		
    	}
        curr = curr->next;
    	
    }
	
	//return that page's frame number 	
    return frame_n;
}



int age(struct frame_struct* frames) {

    
    //return page with lowest counter value
    struct frame_struct* curr = frames;
    while(curr)
    {  // if found send that frames frame number
    	if(curr->pte_pointer == NULL)
    	{
    		
    		return curr->frame_number;
    	}
    	curr= curr->next;
    }
    curr = frames;
    uint8_t min_counter = (uint8_t)255;
    int frame_n = 0;
    struct pte_32* minpte = NULL;
    unsigned int virtual_address = 0;
    while(curr)
    {
    	struct pte_32* newpte = (struct pte_32*)page_table[PTE32_INDEX(curr->virtual_address)];
    	if(newpte->reference_counter < min_counter)
    	{
             min_counter = newpte->reference_counter;
             frame_n = curr->frame_number;
             minpte = newpte;
             virtual_address = curr->virtual_address;
    	}
    	else if(newpte->reference_counter == min_counter)
    	{
    		//check if the page is dirty
    		if(minpte == NULL){
    			min_counter = newpte->reference_counter;
                frame_n = curr->frame_number;
                minpte = newpte;
                virtual_address = curr->virtual_address;
    		}
    		else if( minpte->dirty )
    		{
                if( newpte->dirty)
                {
                	//if both pages are dirty, choose the lowest number
                	if(virtual_address > curr->virtual_address)
                	{
                         min_counter = newpte->reference_counter;
                         frame_n = curr->frame_number;
                         minpte = newpte;
                         virtual_address = curr->virtual_address;
                	}
                }
                //if newpte is not dirty, evict it 
                else
                {
                	min_counter = newpte->reference_counter;
                    frame_n = curr->frame_number;
                    minpte = newpte;
                    virtual_address = curr->virtual_address;
                }
    		}
    		else
    		{
    			//if both pages are not dirty, choose the lowest number
                if(newpte->dirty == 0)
                {
                	//choose the lowest number
                	if(virtual_address > curr->virtual_address)
                	{
                         min_counter = newpte->reference_counter;
                         frame_n = curr->frame_number;
                         minpte = newpte;
                         virtual_address = curr->virtual_address;
                	}
                }
    		}
    		//if(new_pte)
    		
    	}
    	curr= curr->next;
    }

	return frame_n;
}

