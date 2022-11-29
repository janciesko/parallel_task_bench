#!/bin/bash
tail -n +1 powerset-UDR-atomic_blake01_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-nored_blake01_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-taskgroup_blake01_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-threadpriv_blake01_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-parallel_blake01_clang++_overRT.res >> concut/powerset-UDR-clang.res
tail -n +2 powerset-UDR-taskloop_blake01_clang++_overRT.res >> concut/powerset-UDR-clang.res

tail -n +1 powerset-UDR-atomic_blake01_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-parallel_blake01_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-taskloop_blake01_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-nored_blake01_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-taskgroup_blake01_g++_overRT.res >> concut/powerset-UDR-g++.res
tail -n +2 powerset-UDR-threadpriv_blake01_g++_overRT.res >> concut/powerset-UDR-g++.res

