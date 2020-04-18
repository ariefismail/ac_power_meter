/* CircularFifo.h 
 * Not any company's property but Public-Domain
 * Do with source-code as you will. No requirement to keep this
 * header if need to use it/change it/ or do whatever with it
 *
 * Note that there is No guarantee that this code will work
 * and I take no responsibility for this code and any problems you
 * might get if using it. The code is highly platform dependent!
 *
 * Code & platform dependent issues with it was originally
 * published at http://www.kjellkod.cc/threadsafecircularqueue
 * 2009-11-02
 * @author Kjell Hedstr�m, hedstrom@kjellkod.cc */

#ifndef CIRCULARFIFO_H_
#define CIRCULARFIFO_H_

/** Circular Fifo (a.k.a. Circular Buffer) 
 * Thread safe for one reader, and one writer */
template<typename Element, unsigned int Size>
class CCircularFifo
{
public:
	enum
	{
		Capacity = Size + 1
	};

	CCircularFifo() : m_Tail(0), m_Head(0) {}
	virtual ~CCircularFifo() {}

	bool Push(Element& item_);
	bool Pop(Element& item_);

	bool IsEmpty() const;
	bool IsFull() const;
	void Clear()
	{
		m_Tail = 0;
		m_Head = 0;
	}

private:
	volatile unsigned int m_Tail; // input index
	Element m_Array[Capacity];
	volatile unsigned int m_Head; // output index

	unsigned int increment(unsigned int idx_) const;
};

/** Producer only: Adds item to the circular queue. 
 * If queue is full at 'push' operation no update/overwrite
 * will happen, it is up to the caller to handle this case
 *
 * \param item_ copy by reference the input item
 * \return whether operation was successful or not */
template<typename Element, unsigned int Size>
bool CCircularFifo<Element, Size>::Push(Element& item_)
{
	unsigned int nextTail = increment(m_Tail);
	if (nextTail != m_Head)
	{
		m_Array[m_Tail] = item_;
		m_Tail = nextTail;
		return true;
	}

	// queue was full
	return false;
}

/** Consumer only: Removes and returns item from the queue
 * If queue is empty at 'pop' operation no retrieve will happen
 * It is up to the caller to handle this case
 *
 * \param item_ return by reference the wanted item
 * \return whether operation was successful or not */
template<typename Element, unsigned int Size>
bool CCircularFifo<Element, Size>::Pop(Element& item_)
{
	if (m_Head == m_Tail) return false;  // empty queue

	item_ = m_Array[m_Head];
	m_Head = increment(m_Head);
	return true;
}

/** Useful for testing and Consumer check of status
 * Remember that the 'empty' status can change quickly
 * as the Procuder adds more items.
 *
 * \return true if circular buffer is empty */
template<typename Element, unsigned int Size>
bool CCircularFifo<Element, Size>::IsEmpty() const
{
	return (m_Head == m_Tail);
}

/** Useful for testing and Producer check of status
 * Remember that the 'full' status can change quickly
 * as the Consumer catches up.
 *
 * \return true if circular buffer is full.  */
template<typename Element, unsigned int Size>
bool CCircularFifo<Element, Size>::IsFull() const
{
	int tailCheck = (m_Tail + 1) % Capacity;
	return (tailCheck == m_Head);
}

/** Increment helper function for index of the circular queue
 * index is incremented or wrapped
 *
 *  \param idx_ the index to the incremented/wrapped
 *  \return new value for the index */
template<typename Element, unsigned int Size>
unsigned int CCircularFifo<Element, Size>::increment(unsigned int idx_) const
{
	// increment or wrap
	// =================
	//    index++;
	//    if(index == array.lenght) -> index = 0;
	//
	//or as written below:
	//    index = (index+1) % array.length
	idx_ = (idx_ + 1) % Capacity;
	return idx_;
}

#endif /* CIRCULARFIFO_H_ */
