#ifndef CSC443_PROJECT_Clock_H
#define CSC443_PROJECT_Clock_H

#include <vector>
#include <list>
#include "EvictionPolicy.h"
#include "Page.h"

/**
 * Class representing CLOCK eviction policy for buffer pool.
 */
class Clock : public EvictionPolicy {
private:
    std::list<Page *> pages;
    int handle;

public:
    Clock();

    void Insert(Page *page) override;

    void UpdatePageAccessStatus(Page *accessedPage) override;

    Page *GetPageToEvict() override;
};

#endif // CSC443_PROJECT_Clock_H
