#!/bin/bash
tail -n +1 powerset-UDR-atomic_blake23_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-nored_blake23_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-taskgroup_blake23_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-threadpriv_blake23_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-atomic_blake23_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-parallel_blake23_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-taskloop_blake23_clang++_overRT.res >> concut/powerset-UDR-clang.res

tail -n +1 powerset-UDR-atomic_blake23_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-parallel_blake23_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-taskloop_blake23_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-nored_blake23_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-taskgroup_blake23_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-threadpriv_blake23_g++_overRT.res >> concut/powerset-UDR-g++.res

