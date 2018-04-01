#version 450
#define max_expected_neighbors 40

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

layout(std430) restrict coherent buffer Nei
{
    int nei[][max_expected_neighbors];
};

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

void main()
{
    // assumed CCW is: 012.012.0...
    // get IDs of all vertices in current triangle
    int index0 = ID[0];
    int index1 = ID[1];
    int index2 = ID[2];

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
