// Modern C++ Design: Generic Programming and Design Patterns Applied
// Chapter 6

#include <cstdlib>
#include <cassert>
#include <iterator>
#include <algorithm>



namespace Private
{
	class LifetimeTracker
	{
	public:
		LifetimeTracker(unsigned int x) : longevity_(x) {}
		virtual ~LifetimeTracker() = 0;
		friend bool Compare(unsigned int longevity, const LifetimeTracker* p);
	private:
		unsigned int longevity_;
	};

	// Definition required
	inline LifetimeTracker::~LifetimeTracker() {}
	inline bool Compare(unsigned int longevity, const LifetimeTracker *p) {
		return p->longevity_ < longevity;
	}

	typedef LifetimeTracker** TrackerArray;
	extern TrackerArray pTrackerArray;
	extern unsigned int elements;

	//Helper destroyer function
	template <typename T>
	struct Deleter
	{
		static void Delete(T* pObj)
		{
			delete pObj;
		}
	};

	// Concrete lifetime tracker for objects of type T
	template <typename T, typename Destroyer>
	class ConcreteLifetimeTracker : public LifetimeTracker
	{
	public:
		ConcreteLifetimeTracker(T* p, unsigned int longevity, Destroyer d)
			:LifetimeTracker(longevity), pTracked_(p), destroyer_(d) {}
		~ConcreteLifetimeTracker() {
			destroyer_(pTracked_);
		}
	private:
		T * pTracked_;
		Destroyer destroyer_;
	};

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable: 4505)
#endif

	static void AtExitFn() {
		assert(elements > 0 && pTrackerArray != 0);
		// Pick the element at the top of the stack
		LifetimeTracker* pTop = pTrackerArray[elements - 1];
		// Remove that object off the stack
		// Don't check errors-realloc with less memory
		// can't fail
		pTrackerArray = static_cast<TrackerArray>(std::realloc(pTrackerArray, sizeof(*pTrackerArray) * --elements));
		// Destroy the element
		delete pTop;
	}

#ifdef _WIN32
#pragma warning(pop)
#endif

}

template <typename T, typename Destroyer>
void SetLongevity(T* pDynObject, unsigned int longevity,
	Destroyer d = Private::Deleter<T>::Delete)
{
	using namespace Private;

	TrackerArray pNewArray = static_cast<TrackerArray>(
		std::realloc(pTrackerArray, sizeof(*pTrackerArray) *
		(elements + 1)));
	if (!pNewArray) throw std::bad_alloc();
	pTrackerArray = pNewArray;
	LifetimeTracker* p = new ConcreteLifetimeTracker<T, Destroyer>(pDynObject, longevity, d);
	TrackerArray pos = std::upper_bound(pTrackerArray, pTrackerArray + elements, longevity, Compare);

#ifdef _WIN32
	std::copy_backward(pos, pTrackerArray + elements, stdext::make_checked_array_iterator(pTrackerArray + elements + 1, elements + 1));
#else
	std::copy_backward(pos, pTrackerArray + elements, pTrackerArray + elements + 1);
#endif

	*pos = p;
	++elements;
	std::atexit(AtExitFn);
}

