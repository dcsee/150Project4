#include "Machine.h"
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <list>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>

#define BPB_SIZE 55

class ThreadStore;
class TCB;
class Mutex;
class MemoryPool;

typedef struct{
	uint16_t bytesPerSector;
	uint8_t sectorsPerCluster;
	uint16_t reservedSectorCount;
	uint8_t numFATs;
	uint16_t rootEntryCount;
	int totalSectors;
	uint16_t fatSizeSectors;
	uint16_t fatSizeBytes;
} BPB;

typedef struct Directory {
	bool isFile;
	std::string* name;
	SVMDirectoryEntry* selfSVM;
	uint16_t firstClusterLo;
	std::vector<struct Directory*>* childEntries;
} Directory;

typedef struct{

} DirectoryEntry;

typedef struct{
	Mutex *mutex;
	bool waiting;
}	MutexInfo;

typedef struct{
	TCB *thread;
	TVMMemorySize neededSize;
	TVMMemoryPoolID pool;
}	MemoryWaitingInfo;

typedef struct{
	uint8_t *startAddress;
	TVMMemorySize length;
} MemoryChunk;

extern "C"{

//Don't remove or comment out	==================================================================
TVMMainEntry VMLoadModule(const char* module); //They are implemented using Professor Nitta's provided source code in VirtualMachineUtils.c
void VMUnloadModule(void);
//===============================================================================================

void safeEntry(void *param);
void timerInterrupt(void *calldata);	//accepts a VM_THREAD_PRIORITY
void idle(void* parm1);
void machineFileIOCallback(void* calledParam, int result);
int sharedMemoryRead(uint8_t* destination, int fileDescriptor, int start, int length, MemoryPool *sharedMemoryPool);
}

#ifndef FATCONTROLLER_H
#define FATCONTROLLER_H

class FATController{

	public:
		FATController(const char* i);
		const char* getImage();
		void changeDirectory(const char* path);
		int openDirectory(const char* dirName);
		SVMDirectoryEntryRef readDirectory(int dirDescriptor);
		const char* getCurrentDirectoryName();
		bool closeDirectory(int dirDescriptor);
		bool rewindDirectory(int dirDescriptor);
		int openFileInCurrentDirectory(const char* fileName);
		void readFileFromImage(int fileDescriptor, uint8_t *data, int *length, MemoryPool* sharedMemoryPool, TCB* currentThread);
		uint8_t* fat;
		uint8_t* rootDir;
		BPB* bpb;

	private:
		std::string *currentPath;
		void makeBPB();
		void makeFAT();
		void makeRootDir();
		SVMDirectoryEntry* makeSVMDirectoryEntry(int offset);
		Directory* makeDirectoryEntry(int offset, SVMDirectoryEntry* entry);
		const char* makeShortFileName(int offset);
		SVMDateTime* calcDateTimeMasks(uint16_t time, uint16_t date);
		int rootDirSectors;
		int rootDirSizeBytes;
		int startOfDataSectors;
		int startOfDataBytes;
		int directoryLocation;
		const char* image;
		std::vector<Directory*>* directories;
};

#endif

#ifndef MUTEX_H
#define MUTEX_H

class Mutex{
	
	public:
		
		Mutex();
		~Mutex();
		TCB* getOwner();
		bool isDeleted();
		TVMMutexID getID();
		void deleteFromVM();
		void lock(TCB* thread, TVMTick tick);
		void release();

	private:
		TVMMutexID id;
		bool deleted;
		TCB *owner;	
		std::list<TCB*>* priorityLists[3];
};

#endif


#ifndef TCB_H
#define TCB_H

class TCB{

	public:
		TCB(TVMThreadEntry entry, void *param, TVMMemorySize memsize, TVMThreadPriority prio, TVMThreadIDRef tid);
		~TCB();
		TVMThreadPriority getPriority();
		TVMThreadID getThreadID();
		void sleep(TVMTick tick);
		TVMThreadState getState();
		void setState(TVMThreadState state);
		void addMutex(Mutex *m, bool w);
		void setWaitingOnMutex(bool waiting);
		Mutex* getWaitingMutex();					//returns the tail of the mutexInfo list
		void removeMutex(Mutex *m);				//remove specified mutex from list
		bool isWaitingOnMutex();					//returns if TCB is waiting on last added mutex
		Mutex* getMutex();
		TVMTick getTicksToSleep();
		void decrementTicksToSleep();
		void setFileIOResult(int result);
		int getFileIOResult();	
		bool isWaitingOnIO();
		void setIsWaitingOnIO(bool waiting);
		bool isDeleted();
		void setIsDeleted(bool d);
		TVMThreadEntry getEntryFunction();
		SMachineContextRef contextRef;
		void releaseMutexes();
		void setWaitingMemoryResult(uint8_t*);
		uint8_t* getWaitingMemoryResult();

	private:
		TVMThreadEntry safeEntryFunction;		
		int fileIOResult;
		int** safeEntryParam;
		TVMThreadID threadID;
		TVMThreadPriority priority;
		TVMThreadState state;
		TVMThreadEntry entryFunction;
		bool deleted;
		void* entryParam;
		void* stackBaseAddress;
		TVMMemorySize stackSize;
		TVMTick ticksToSleep;						//how many ticks to sleep the thread before awaking it
		bool waitingOnIO;
		uint8_t* waitingMemoryResult;
		std::list<MutexInfo*> *mutexes;
};

#endif

#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

class MemoryPool{

	public:
		MemoryPool(uint8_t *baseAddress, TVMMemorySize size);
		~MemoryPool();
		TVMMemoryPoolID getID();
		uint8_t* allocateMemory(TVMMemorySize size);
		bool deallocate(uint8_t* address);
		TVMMemorySize getSize();
		bool isAddressInRange(uint8_t* address);
		bool isInUse();
		TVMMemorySize getNumberOfUnallocatedBytes();

	private:
		uint8_t* getNextSpace(TVMMemorySize size);
		void addChunk(MemoryChunk *chunk);
		uint8_t *baseAddress;
		TVMMemorySize poolSize;
		TVMMemoryPoolID id;
		std::vector<MemoryChunk*>* allocatedChunkVector;
};

#endif


#ifndef THREADSTORE_H
#define THREADSTORE_H

class ThreadStore{
	
	public:
		//Other
		static ThreadStore *getInstance();
		~ThreadStore();
		void timerEvent();
		int voidParam;
	
		//Mutex	
		Mutex* findMutexByID(TVMMutexID mutexID);
		TVMMutexID getNumMutexes();
		void insert(Mutex *mutex);

		//Threads
		TCB* findThreadByID(TVMThreadID ID);
		TCB* getCurrentThread();
		void insert(TCB *tcb);
		int getNumThreads();
		void createIdleThread();
		void createMainThread();
		void sleepCurrentThread(TVMTick tick);
		void scheduleThreadEarly(TCB* thread);
		void runThreadEarly(TCB* thread);
		void schedule(TVMThreadPriority priority);
		void activateDeadThread(TCB* deadThread);
		bool isThreadPrioInRange(TCB*);	//debug, remove
		void switchToNewThread(TCB* thread);
		void waitCurrentThreadOnIO();
		void removeFromWaitlistEarly(TCB *thread);
		void terminate(TCB* thread);
		void terminateCurrentThread();
		void deleteDeadThread(TCB* thread);

		//FAT
		void createFATController(const char* image);
		FATController* getFATController();	
	
		//Memory Pools
		MemoryPool* findMemoryPoolByID(TVMMemoryPoolID memoryPoolID);
		TVMMemoryPoolID getNumMemoryPools();
		void insert(MemoryPool *memoryPool);
		void setSharedMemoryParams(void *sharedMemoryBaseAddress, TVMMemorySize sharedsize);
		void createSystemMemoryPool(TVMMemorySize heapSize);
		void createSharedMemoryPool(uint8_t *baseAddress, TVMMemorySize sharedSize);
		uint8_t* waitCurrentThreadOnMemory(TVMMemorySize size, TVMMemoryPoolID mid);
		void signalMemoryRelease(TVMMemorySize size, TVMMemoryPoolID mid);
		void deleteMemoryPool(TVMMemoryPoolID poolID);

	protected:

		ThreadStore();
		
		FATController *fatController;
		uint8_t *systemMemoryBaseAddress;
		static ThreadStore *DUniqueInstance;
		TVMMemoryPoolID numMemoryPools;
		TVMThreadID numThreads;
		TVMMutexID numMutexes;
		TCB* idleThread;
		TCB* currentThread;
		TVMThreadID idleThreadID;
		std::list<TCB*>* readyLists[4];
		std::list<MemoryWaitingInfo*>* memoryWaitingLists[3];
		std::list<TCB*> *waitList;
		std::list<TCB*> *deadList;
		std::vector<TCB*> *allThreads;
		std::vector<Mutex*> *mutexVector;
		std::vector<MemoryPool*> *memoryPoolVector;
};

#endif
