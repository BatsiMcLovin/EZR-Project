#version 450
#define max_expected_neighbors 40

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std430, binding = 0) restrict coherent buffer Nei
{
    int nei[][max_expected_neighbors];
};

layout(std430, binding = 1) restrict coherent buffer Counters
{
    int counters[];
};

/*
*/


// THIS MIGHT BE USELESS LATER
layout(std430) restrict coherent buffer Semaphore
{
    int semaphore[];
};


in int ID[3];


bool handleInsertion(int nei_list, int candidate_1, int candidate_2)
{
	int nextFreeIndex = nei[nei_list][max_expected_neighbors - 1]; 	//read next free index
    nei[nei_list][nextFreeIndex] = candidate_1;	// candidate 1 will be set as our vertex's neighbor
    nextFreeIndex++;
    nei[nei_list][nextFreeIndex] = candidate_2; // same for candidate 2
    nextFreeIndex++; // don't now why this is done...
    nei[nei_list][max_expected_neighbors - 1] = nextFreeIndex; //last entry of nei saves the latest non-written index 
    return true;
}

bool handleInsertionRaphiStyle(int superindex, int vertex, int neighbor_1, int neighbor_2)
{
	int indx0 = superindex * 2;
	int indx1 = indx0 + 1;

	if(indx0 >= (max_expected_neighbors - 4) || indx1 >= (max_expected_neighbors - 4))
		return false;

	nei[vertex][indx0] = neighbor_1;
	nei[vertex][indx1] = neighbor_2;

	return true;
}

void main()
{
    // assumed CCW is: 012.012.0...
    // get IDs of all vertices in current triangle
    int index0 = ID[0];
    int index1 = ID[1];
    int index2 = ID[2];


	// this is the superindex of the insertion (i can calculate the exact location to write my data, since we're talking about triangles)
	int index00 = atomicAdd(counters[index0], 1);
	memoryBarrier();
	int index01 = atomicAdd(counters[index1], 1);
	memoryBarrier();	
	int index02 = atomicAdd(counters[index2], 1);
	memoryBarrier();

	// handle insertions
	handleInsertionRaphiStyle(index00, index0, index1, index2);
	handleInsertionRaphiStyle(index01, index1, index2, index0);
	handleInsertionRaphiStyle(index02, index2, index0, index1);

	//just to make sure they're all on the same level
	memoryBarrier();

	if(index00 == 0)
	{
		nei[index0][max_expected_neighbors - 1] = abs(counters[index0] * 2);	
		memoryBarrier();
	}

	if(index01 == 0)
	{	
		nei[index1][max_expected_neighbors - 1] = counters[index1] * 2;
		memoryBarrier();
	}

	if(index02 == 0)
	{
		nei[index2][max_expected_neighbors - 1] = counters[index2] * 2;
		memoryBarrier();
	}
	memoryBarrier();

	return; // DO NOT REMOVE THIS RETURN (MEMORYBARRIERs MIGHT LEAD TO AN ENDLESS LOOP)



	// EVERYTHING FROM HERE ON IS NILS' CODE. will not be executed ( but preserved until everything is bugfree)

	// per vertex check if job is done
    bool done0 = false;
    bool done1 = false;
    bool done2 = false;

    int locked0 = 0;
    int locked1 = 0;
    int locked2 = 0;




    while(!done0 || !done1 || !done2)
    {
        if(!done0)
        {
            locked0 = atomicCompSwap(semaphore[index0], 0, 1); // if condition will be locked
            if (locked0 == 0)
            {
                handleInsertion(index0, index1, index2);

                memoryBarrier();
                done0 = true;
                atomicExchange(semaphore[index0], 0); // now other threads can execute this scope
            }
        }
        if(!done1)
        {
            locked1 = atomicCompSwap(semaphore[index1], 0, 1);
            if (locked1 == 0)
            {
                handleInsertion(index1, index2, index0);

                memoryBarrier();
                done1 = true;
                atomicExchange(semaphore[index1], 0);
            }
        }
        if(!done2)
        {
            locked2 = atomicCompSwap(semaphore[index2], 0, 1);
            if (locked2 == 0)
            {
                handleInsertion(index2, index0, index1);

                memoryBarrier();
                done2 = true;
                atomicExchange(semaphore[index2], 0);
            }
        }
    }

}
