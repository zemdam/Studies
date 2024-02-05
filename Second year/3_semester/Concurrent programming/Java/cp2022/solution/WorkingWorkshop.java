package cp2022.solution;

import cp2022.base.Workplace;
import cp2022.base.WorkplaceId;
import cp2022.base.Workshop;

import java.util.Collection;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.Semaphore;
import java.util.stream.Collectors;

public class WorkingWorkshop implements Workshop {

    private final Map<WorkplaceId, WorkplaceDecorator> workplaces;
    private final Map<WorkplaceId, Semaphore> workplaceMutex;
    private final Map<WorkplaceId, Semaphore> enterSemaphore;
    private final Map<WorkplaceId, Semaphore> workSemaphore;
    private final Map<WorkplaceId, Semaphore> switchSemaphore;
    private final Map<WorkplaceId, Integer> inWorkplace;
    private final Map<WorkplaceId, Integer> toEnter;
    private final Map<WorkplaceId, Integer> toSwitch;
    private final ThreadLocal<WorkplaceId> currentWorkplaceId;
    private final ThreadLocal<WorkplaceId> previousWorkplaceId;
    private final Semaphore mutex;
    private final Map<Long, Semaphore> threadsSemaphore;
    private final int max;

    private class WorkplaceDecorator extends Workplace {
        private final Workplace workplace;

        private WorkplaceDecorator(Workplace workplace) {
            super(workplace.getId());
            this.workplace = workplace;
        }

        @Override
        public void use() {
            try {
                if (previousWorkplaceId.get() != null) {
                    workSemaphore.get(previousWorkplaceId.get()).release();
                    //System.out.println(Thread.currentThread().getId() + " Relase: " + previousWorkplaceId.get());
                }

                mutex.acquire();
                Semaphore tmp = threadsSemaphore.get(Thread.currentThread().getId());
                tmp.release(tmp.getQueueLength() + max);
                mutex.release();

                //System.out.println(Thread.currentThread().getId() + " acquire: " + workplace.getId());
                workSemaphore.get(workplace.getId()).acquire();
                workplace.use();
            } catch (InterruptedException e) {
                throw new RuntimeException("panic: unexpected thread interruption");
            }
        }
    }

    public WorkingWorkshop(Collection<Workplace> workplaces) {
        this.workplaces = workplaces.stream().collect(Collectors.toConcurrentMap(Workplace::getId, WorkplaceDecorator::new));
        currentWorkplaceId = new ThreadLocal<>();
        previousWorkplaceId = new ThreadLocal<>();
        mutex = new Semaphore(1, true);
        threadsSemaphore = new HashMap<>();
        max = 2 * workplaces.size() - 1;

        workplaceMutex = new ConcurrentHashMap<>();
        enterSemaphore = new ConcurrentHashMap<>();
        workSemaphore = new ConcurrentHashMap<>();
        switchSemaphore = new ConcurrentHashMap<>();
        inWorkplace = new ConcurrentHashMap<>();
        toEnter = new ConcurrentHashMap<>();
        toSwitch = new ConcurrentHashMap<>();
        for (Workplace workplace : workplaces) {
            workplaceMutex.computeIfAbsent(workplace.getId(), key -> new Semaphore(1, true));
            enterSemaphore.computeIfAbsent(workplace.getId(), key -> new Semaphore(0, true));
            workSemaphore.computeIfAbsent(workplace.getId(), key -> new Semaphore(1, true));
            switchSemaphore.computeIfAbsent(workplace.getId(), key -> new Semaphore(0, true));
            inWorkplace.computeIfAbsent(workplace.getId(), key -> 0);
            toEnter.computeIfAbsent(workplace.getId(), key -> 0);
            toSwitch.computeIfAbsent(workplace.getId(), key -> 0);
        }
    }

    @Override
    public Workplace enter(WorkplaceId wid) {
        try {
            mutex.acquire();
            Map<Long, Semaphore> threadsSemaphoreCopy = new HashMap<>(threadsSemaphore);
            threadsSemaphore.put(Thread.currentThread().getId(), new Semaphore(max));
            mutex.release();

            for (Map.Entry<Long, Semaphore> entry : threadsSemaphoreCopy.entrySet()) {
                mutex.acquire();
                if (threadsSemaphore.containsKey(entry.getKey())) {
                    mutex.release();
                    entry.getValue().acquire();
                } else {
                    mutex.release();
                }
            }

            previousWorkplaceId.set(null);
            workplaceMutex.get(wid).acquire();
            if (inWorkplace.get(wid) + toSwitch.get(wid) > 0) {
                toEnter.computeIfPresent(wid, (k,v) -> v + 1);
                workplaceMutex.get(wid).release();
                enterSemaphore.get(wid).acquire();
                toEnter.computeIfPresent(wid, (k,v) -> v - 1);
            } else {
                inWorkplace.computeIfPresent(wid, (k,v) -> v + 1);
            }
            workplaceMutex.get(wid).release();

            currentWorkplaceId.set(wid);
        } catch (InterruptedException e) {
            throw new RuntimeException("panic: unexpected thread interruption");
        }
        return workplaces.get(wid);
    }

    @Override
    public Workplace switchTo(WorkplaceId wid) {
        try {
            previousWorkplaceId.set(currentWorkplaceId.get());
            currentWorkplaceId.set(wid);
            if (previousWorkplaceId.get() == wid) {
                return workplaces.get(wid);
            }


            workplaceMutex.get(previousWorkplaceId.get()).acquire();
            if (toSwitch.get(previousWorkplaceId.get()) > 0) {
                switchSemaphore.get(previousWorkplaceId.get()).release();
            } else if (toEnter.get(previousWorkplaceId.get()) > 0) {
                enterSemaphore.get(previousWorkplaceId.get()).release();
            } else {
                inWorkplace.computeIfPresent(previousWorkplaceId.get(), (k,v) -> v - 1);
                workplaceMutex.get(previousWorkplaceId.get()).release();
            }

            workplaceMutex.get(wid).acquire();
            if (inWorkplace.get(wid) > 0) {
                toSwitch.computeIfPresent(wid, (k,v) -> v + 1);
                workplaceMutex.get(wid).release();
                switchSemaphore.get(wid).acquire();
                toSwitch.computeIfPresent(wid, (k,v) -> v - 1);
            } else {
                inWorkplace.computeIfPresent(wid, (k,v) -> v + 1);
            }
            workplaceMutex.get(wid).release();

        } catch (InterruptedException e) {
            throw new RuntimeException("panic: unexpected thread interruption");
        }
        return workplaces.get(wid);
    }

    @Override
    public void leave() {
        try {
            mutex.acquire();
            Semaphore tmp = threadsSemaphore.get(Thread.currentThread().getId());
            tmp.release(tmp.getQueueLength() + max);
            threadsSemaphore.remove(Thread.currentThread().getId());
            mutex.release();

            workplaceMutex.get(currentWorkplaceId.get()).acquire();
            if (toSwitch.get(currentWorkplaceId.get()) > 0) {
                switchSemaphore.get(currentWorkplaceId.get()).release();
            } else if (toEnter.get(currentWorkplaceId.get()) > 0) {
                enterSemaphore.get(currentWorkplaceId.get()).release();
            } else {
                inWorkplace.computeIfPresent(currentWorkplaceId.get(), (k,v) -> v - 1);
                workplaceMutex.get(currentWorkplaceId.get()).release();
            }

            workSemaphore.get(currentWorkplaceId.get()).release();
        } catch (InterruptedException e) {
            throw new RuntimeException("panic: unexpected thread interruption");
        }
    }
}
