! MMS/MMK build file
! Use MMS/EXT if you build with MMS.

LIBUV = libuv.olb

! Currently clang is required for <stdatomic>
CC = CXX

CFLAGS = $(CFLAGS)/Debug/Opt=(Lev=4)/Reentr=Multi/NoExcept/NoRTTI/Pointer=64=argv-
  /DEFINE=(_USE_STD_STAT, _POSIX_EXIT, _SOCKADDR_LEN)/NoANSI_Alias-
  /INCLUDE=([-.include],[-.include.uv],[-.src],[-.src.unix])-
  /WARN=(DISABLE="c++11-compat-deprecated-writable-strings")

all : $(LIBUV), uv_run_benchmarks.exe, uv_run_tests.exe

libuv.olb : libuv.olb(fs-poll=fs-poll.obj), libuv.olb(idna=idna.obj),-
        libuv.olb(inet=inet.obj), libuv.olb(random=random.obj),-
        libuv.olb(strscpy=strscpy.obj), libuv.olb(strtok=strtok.obj),-
        libuv.olb(thread-common=thread-common.obj),-
        libuv.olb(threadpool=threadpool.obj), libuv.olb(timer=timer.obj),-
        libuv.olb(uv-common=uv-common.obj), libuv.olb(version=version.obj),-
        libuv.olb(uv-data-getter-setters=uv-data-getter-setters.obj),-
        libuv.olb(vms-async=vms-async.obj), libuv.olb(core=core.obj),-
        libuv.olb(dl=dl.obj), libuv.olb(thread.obj),-
        libuv.olb(fs=fs.obj), libuv.olb(poll=poll.obj),-
        libuv.olb(getaddrinfo=getaddrinfo.obj),-
        libuv.olb(getnameinfo=getnameinfo.obj),-
        libuv.olb(loop-watcher=loop-watcher.obj), libuv.olb(tty=tty.obj),-
        libuv.olb(loop=loop.obj), libuv.olb(pipe=pipe.obj),-
        libuv.olb(tcp=tcp.obj), libuv.olb(udp=udp.obj),-
        libuv.olb(vms=vms.obj), libuv.olb(stream=stream.obj),-
        libuv.olb(vms-syscalls=vms-syscalls.obj),-
        libuv.olb(process=process.obj), libuv.olb(signal=signal.obj),-
        libuv.olb(no-fsevents=no-fsevents.obj),-
        libuv.olb(no-proctitle=no-proctitle.obj),-
        libuv.olb(vms-poll=vms-poll.obj),-
        libuv.olb(vms-error=vms-error.obj), libuv.olb(bsd-ifaddrs=bsd-ifaddrs.obj)
    @ continue

uv_run_benchmarks.exe : benchmark-async.obj, benchmark-async-pummel.obj,-
                benchmark-fs-stat.obj, benchmark-getaddrinfo.obj,-
                benchmark-loop-count.obj, benchmark-million-async.obj,-
                benchmark-million-timers.obj, benchmark-multi-accept.obj,-
                benchmark-ping-pongs.obj, benchmark-ping-udp.obj,-
                benchmark-pound.obj, benchmark-pump.obj,-
                benchmark-queue-work.obj, benchmark-sizes.obj,-
                benchmark-spawn.obj, benchmark-tcp-write-batch.obj,-
                benchmark-thread.obj, benchmark-udp-pummel.obj,-
                runner.obj, run-benchmarks.obj, echo-server.obj,-
                blackhole-server.obj, runner-unix.obj, libuv.olb
        LINK/Trace/NoDebug/Threads/EXE=uv_run_benchmarks.exe -
                $(MMS$SOURCE_LIST)/LIBRARY

uv_run_tests.exe :  blackhole-server.obj, echo-server.obj, run-tests.obj,-
                runner.obj, runner-unix.obj,-
                test-active.obj, test-async-null-cb.obj,-
                test-async.obj, test-barrier.obj, test-callback-stack.obj,-
                test-close-fd.obj, test-close-order.obj, test-condvar.obj,-
                test-connect-unspecified.obj, test-connection-fail.obj,-
                test-cwd-and-chdir.obj, test-default-loop-close.obj,-
                test-delayed-accept.obj, test-dlerror.obj,-
                test-eintr-handling.obj, test-embed.obj,-
                test-emfile.obj, test-env-vars.obj, test-error.obj,-
                test-fail-always.obj, test-fork.obj, test-fs-copyfile.obj,-
                test-fs-event.obj, test-fs-poll.obj, test-fs.obj,-
                test-fs-readdir.obj, test-fs-fd-hash.obj, test-fs-open-flags.obj,-
                test-get-currentexe.obj, test-get-loadavg.obj, test-get-memory.obj,-
                test-get-passwd.obj, test-getaddrinfo.obj, test-gethostname.obj,-
                test-getnameinfo.obj, test-getsockname.obj, test-getters-setters.obj,-
                test-gettimeofday.obj, test-handle-fileno.obj, test-homedir.obj,-
                test-hrtime.obj, test-idle.obj, test-idna.obj, test-ip4-addr.obj,-
                test-ip6-addr.obj, test-ip-name.obj, test-ipc-send-recv.obj,-
                test-ipc-heavy-traffic-deadlock-bug.obj, test-ipc.obj,-
                test-loop-alive.obj, test-loop-close.obj, test-loop-configure.obj,-
                test-loop-handles.obj, test-loop-stop.obj, test-loop-time.obj,-
                test-metrics.obj, test-multiple-listen.obj, test-mutexes.obj,-
                test-not-readable-nor-writable-on-read-error.obj,-
                test-not-writable-after-shutdown.obj, test-osx-select.obj,-
                test-pass-always.obj, test-ping-pong.obj, test-pipe-bind-error.obj,-
                test-pipe-close-stdout-read-stdin.obj, test-pipe-connect-error.obj,-
                test-pipe-connect-multiple.obj, test-pipe-connect-prepare.obj,-
                test-pipe-getsockname.obj, test-pipe-pending-instances.obj,-
                test-pipe-sendmsg.obj, test-pipe-server-close.obj,-
                test-pipe-set-fchmod.obj, test-pipe-set-non-blocking.obj,-
                test-platform-output.obj, test-poll-close-doesnt-corrupt-stack.obj,-
                test-poll-close.obj, test-poll-closesocket.obj, test-poll-oob.obj,-
                test-poll-multiple-handles.obj, test-poll.obj, test-process-priority.obj,-
                test-process-title-threadsafe.obj, test-process-title.obj,-
                test-queue-foreach-delete.obj, test-random.obj, test-readable-on-eof.obj,-
                test-ref.obj, test-run-nowait.obj, test-run-once.obj, test-semaphore.obj,-
                test-shutdown-close.obj, test-shutdown-eof.obj, test-shutdown-twice.obj,-
                test-shutdown-simultaneous.obj, test-signal-multiple-loops.obj,-
                test-signal-pending-on-close.obj, test-signal.obj, test-spawn.obj,-
                test-socket-buffer-size.obj, test-stdio-over-pipes.obj, test-strscpy.obj,-
                test-strtok.obj, test-tcp-alloc-cb-fail.obj, test-tcp-bind-error.obj,-
                test-tcp-bind6-error.obj, test-tcp-close-accept.obj, test-tcp-close.obj,-
                test-tcp-close-after-read-timeout.obj, test-tcp-close-while-connecting.obj,-
                test-tcp-close-reset.obj, test-tcp-connect-error-after-write.obj,-
                test-tcp-connect-error.obj, test-tcp-connect-timeout.obj,-
                test-tcp-connect6-error.obj, test-tcp-create-socket-early.obj,-
                test-tcp-flags.obj, test-tcp-oob.obj, test-tcp-open.obj, test-tcp-read-stop.obj,-
                test-tcp-read-stop-start.obj, test-tcp-rst.obj, test-tcp-try-write.obj,-
                test-tcp-shutdown-after-write.obj, test-tcp-write-in-a-row.obj,-
                test-tcp-try-write-error.obj, test-tcp-unexpected-read.obj,-
                test-tcp-write-after-connect.obj, test-tcp-write-fail.obj,-
                test-tcp-write-queue-order.obj, test-tcp-write-to-half-open-connection.obj,-
                test-tcp-writealot.obj, test-test-macros.obj, test-thread-affinity.obj,-
                test-thread-equal.obj, test-thread.obj, test-threadpool-cancel.obj,-
                test-threadpool.obj, test-timer-again.obj, test-timer-from-check.obj,-
                test-timer.obj, test-tmpdir.obj, test-tty-duplicate-key.obj,-
                test-tty-escape-sequence-processing.obj, test-tty.obj,-
                test-udp-alloc-cb-fail.obj, test-udp-bind.obj, test-udp-connect.obj,-
                test-udp-connect6.obj, test-udp-create-socket-early.obj,-
                test-udp-dgram-too-big.obj, test-udp-ipv6.obj, test-udp-mmsg.obj,-
                test-udp-multicast-interface.obj, test-udp-multicast-interface6.obj,-
                test-udp-multicast-join.obj, test-udp-multicast-join6.obj,-
                test-udp-multicast-ttl.obj, test-udp-open.obj, test-udp-options.obj,-
                test-udp-send-and-recv.obj, test-udp-send-hang-loop.obj,-
                test-udp-send-immediate.obj, test-udp-sendmmsg-error.obj,-
                test-udp-send-unreachable.obj, test-udp-try-send.obj,-
                test-udp-recv-in-a-row.obj, test-uname.obj, test-walk-handles.obj,-
                test-watcher-cross-stop.obj, libuv.olb
        LINK/Trace/NoDebug/Threads/EXE=uv_run_tests.exe -
                $(MMS$SOURCE_LIST)/LIBRARY


COMMON_H = [-.include]uv.h, [-.include.uv]unix.h, [-.include.uv]threadpool.h,-
        [-.include.uv]uv-errno.h,-
        [-.src]heap-inl.h, [-.src]idna.h, [-.src]queue.h, [-.src]strscpy.h,-
        [-.src]strtok.h, [-.src]uv-common.h, [-.src.unix]internal.h

fs-poll.obj                 : [-.src]fs-poll.c, $(COMMON_H)
idna.obj                    : [-.src]idna.c, $(COMMON_H)
inet.obj                    : [-.src]inet.c, $(COMMON_H)
random.obj                  : [-.src]random.c, $(COMMON_H)
strscpy.obj                 : [-.src]strscpy.c, $(COMMON_H)
strtok.obj                  : [-.src]strtok.c, $(COMMON_H)
thread-common.obj           : [-.src]thread-common.c, $(COMMON_H)
threadpool.obj              : [-.src]threadpool.c, $(COMMON_H)
timer.obj                   : [-.src]timer.c, $(COMMON_H)
uv-common.obj               : [-.src]uv-common.c, $(COMMON_H)
uv-data-getter-setters.obj  : [-.src]uv-data-getter-setters.c, $(COMMON_H)
version.obj                 : [-.src]version.c, $(COMMON_H)

bsd-ifaddrs.obj             : [-.src.unix]bsd-ifaddrs.c, $(COMMON_H)
core.obj                    : [-.src.unix]core.c, $(COMMON_H)
dl.obj                      : [-.src.unix]dl.c, $(COMMON_H)
fs.obj                      : [-.src.unix]fs.c, $(COMMON_H)
getaddrinfo.obj             : [-.src.unix]getaddrinfo.c, $(COMMON_H)
getnameinfo.obj             : [-.src.unix]getnameinfo.c, $(COMMON_H)
loop-watcher.obj            : [-.src.unix]loop-watcher.c, $(COMMON_H)
loop.obj                    : [-.src.unix]loop.c, $(COMMON_H)
no-fsevents.obj             : [-.src.unix]no-fsevents.c, $(COMMON_H)
no-proctitle.obj            : [-.src.unix]no-proctitle.c, $(COMMON_H)
pipe.obj                    : [-.src.unix]pipe.c, $(COMMON_H)
poll.obj                    : [-.src.unix]poll.c, $(COMMON_H)
process.obj                 : [-.src.unix]process.c, $(COMMON_H)
signal.obj                  : [-.src.unix]signal.c, $(COMMON_H)
stream.obj                  : [-.src.unix]stream.c, $(COMMON_H)
thread.obj                  : [-.src.unix]thread.c, $(COMMON_H)
tcp.obj                     : [-.src.unix]tcp.c, $(COMMON_H)
tty.obj                     : [-.src.unix]tty.c, $(COMMON_H)
udp.obj                     : [-.src.unix]udp.c, $(COMMON_H)
vms.obj                     : [-.src.unix]vms.c, $(COMMON_H)
vms-async.obj               : [-.src.unix]vms-async.c, $(COMMON_H)
vms-error.obj               : [-.src.unix]vms-error.c, $(COMMON_H)
vms-poll.obj                : [-.src.unix]vms-poll.c, $(COMMON_H)
vms-syscalls.obj            : [-.src.unix]vms-syscalls.c, $(COMMON_H)

benchmark-async.obj         : [-.test]benchmark-async.c, $(COMMON_H)
benchmark-async-pummel.obj  : [-.test]benchmark-async-pummel.c, $(COMMON_H)
benchmark-fs-stat.obj       : [-.test]benchmark-fs-stat.c, $(COMMON_H)
benchmark-getaddrinfo.obj   : [-.test]benchmark-getaddrinfo.c, $(COMMON_H)
benchmark-loop-count.obj    : [-.test]benchmark-loop-count.c, $(COMMON_H)
benchmark-million-async.obj : [-.test]benchmark-million-async.c, $(COMMON_H)
benchmark-million-timers.obj : [-.test]benchmark-million-timers.c, $(COMMON_H)
benchmark-multi-accept.obj  : [-.test]benchmark-multi-accept.c, $(COMMON_H)
benchmark-ping-pongs.obj    : [-.test]benchmark-ping-pongs.c, $(COMMON_H)
benchmark-ping-udp.obj      : [-.test]benchmark-ping-udp.c, $(COMMON_H)
benchmark-pound.obj         : [-.test]benchmark-pound.c, $(COMMON_H)
benchmark-pump.obj          : [-.test]benchmark-pump.c, $(COMMON_H)
benchmark-queue-work.obj    : [-.test]benchmark-queue-work.c, $(COMMON_H)
benchmark-sizes.obj         : [-.test]benchmark-sizes.c, $(COMMON_H)
benchmark-spawn.obj         : [-.test]benchmark-spawn.c, $(COMMON_H)
benchmark-tcp-write-batch.obj : [-.test]benchmark-tcp-write-batch.c, $(COMMON_H)
benchmark-thread.obj        : [-.test]benchmark-thread.c, $(COMMON_H)
benchmark-udp-pummel.obj    : [-.test]benchmark-udp-pummel.c, $(COMMON_H)
blackhole-server.obj        : [-.test]blackhole-server.c, $(COMMON_H)
echo-server.obj             : [-.test]echo-server.c, $(COMMON_H)
runner.obj                  : [-.test]runner.c, $(COMMON_H)
runner-unix.obj             : [-.test]runner-unix.c, $(COMMON_H)
run-benchmarks.obj          : [-.test]run-benchmarks.c, $(COMMON_H)

run-tests.obj               : [-.test]run-tests.c, $(COMMON_H)
test-active.obj             : [-.test]test-active.c, $(COMMON_H)
test-async-null-cb.obj      : [-.test]test-async-null-cb.c, $(COMMON_H)
test-async.obj              : [-.test]test-async.c, $(COMMON_H)
test-barrier.obj            : [-.test]test-barrier.c, $(COMMON_H)
test-callback-stack.obj     : [-.test]test-callback-stack.c, $(COMMON_H)
test-close-fd.obj           : [-.test]test-close-fd.c, $(COMMON_H)
test-close-order.obj        : [-.test]test-close-order.c, $(COMMON_H)
test-condvar.obj            : [-.test]test-condvar.c, $(COMMON_H)
test-connect-unspecified.obj : [-.test]test-connect-unspecified.c, $(COMMON_H)
test-connection-fail.obj    : [-.test]test-connection-fail.c, $(COMMON_H)
test-cwd-and-chdir.obj      : [-.test]test-cwd-and-chdir.c, $(COMMON_H)
test-default-loop-close.obj : [-.test]test-default-loop-close.c, $(COMMON_H)
test-delayed-accept.obj     : [-.test]test-delayed-accept.c, $(COMMON_H)
test-dlerror.obj            : [-.test]test-dlerror.c, $(COMMON_H)
test-eintr-handling.obj     : [-.test]test-eintr-handling.c, $(COMMON_H)
test-embed.obj              : [-.test]test-embed.c, $(COMMON_H)
test-emfile.obj             : [-.test]test-emfile.c, $(COMMON_H)
test-env-vars.obj           : [-.test]test-env-vars.c, $(COMMON_H)
test-error.obj              : [-.test]test-error.c, $(COMMON_H)
test-fail-always.obj        : [-.test]test-fail-always.c, $(COMMON_H)
test-fork.obj               : [-.test]test-fork.c, $(COMMON_H)
test-fs-copyfile.obj        : [-.test]test-fs-copyfile.c, $(COMMON_H)
test-fs-event.obj           : [-.test]test-fs-event.c, $(COMMON_H)
test-fs-poll.obj            : [-.test]test-fs-poll.c, $(COMMON_H)
test-fs-readdir.obj         : [-.test]test-fs-readdir.c, $(COMMON_H)
test-fs-fd-hash.obj         : [-.test]test-fs-fd-hash.c, $(COMMON_H)
test-fs-open-flags.obj      : [-.test]test-fs-open-flags.c, $(COMMON_H)
test-fs.obj                 : [-.test]test-fs.c, $(COMMON_H)
test-get-currentexe.obj     : [-.test]test-get-currentexe.c, $(COMMON_H)
test-get-loadavg.obj        : [-.test]test-get-loadavg.c, $(COMMON_H)
test-get-memory.obj         : [-.test]test-get-memory.c, $(COMMON_H)
test-get-passwd.obj         : [-.test]test-get-passwd.c, $(COMMON_H)
test-getaddrinfo.obj        : [-.test]test-getaddrinfo.c, $(COMMON_H)
test-gethostname.obj        : [-.test]test-gethostname.c, $(COMMON_H)
test-getnameinfo.obj        : [-.test]test-getnameinfo.c, $(COMMON_H)
test-getsockname.obj        : [-.test]test-getsockname.c, $(COMMON_H)
test-getters-setters.obj    : [-.test]test-getters-setters.c, $(COMMON_H)
test-gettimeofday.obj       : [-.test]test-gettimeofday.c, $(COMMON_H)
test-handle-fileno.obj      : [-.test]test-handle-fileno.c, $(COMMON_H)
test-homedir.obj            : [-.test]test-homedir.c, $(COMMON_H)
test-hrtime.obj             : [-.test]test-hrtime.c, $(COMMON_H)
test-idle.obj               : [-.test]test-idle.c, $(COMMON_H)
test-idna.obj               : [-.test]test-idna.c, $(COMMON_H)
test-ip4-addr.obj           : [-.test]test-ip4-addr.c, $(COMMON_H)
test-ip6-addr.obj           : [-.test]test-ip6-addr.c, $(COMMON_H)
test-ip-name.obj            : [-.test]test-ip-name.c, $(COMMON_H)
test-ipc-heavy-traffic-deadlock-bug.obj -
        : [-.test]test-ipc-heavy-traffic-deadlock-bug.c, $(COMMON_H)
test-ipc-send-recv.obj      : [-.test]test-ipc-send-recv.c, $(COMMON_H)
test-ipc.obj                : [-.test]test-ipc.c, $(COMMON_H)
test-loop-alive.obj         : [-.test]test-loop-alive.c, $(COMMON_H)
test-loop-close.obj         : [-.test]test-loop-close.c, $(COMMON_H)
test-loop-configure.obj     : [-.test]test-loop-configure.c, $(COMMON_H)
test-loop-handles.obj       : [-.test]test-loop-handles.c, $(COMMON_H)
test-loop-stop.obj          : [-.test]test-loop-stop.c, $(COMMON_H)
test-loop-time.obj          : [-.test]test-loop-time.c, $(COMMON_H)
test-metrics.obj            : [-.test]test-metrics.c, $(COMMON_H)
test-multiple-listen.obj    : [-.test]test-multiple-listen.c, $(COMMON_H)
test-mutexes.obj            : [-.test]test-mutexes.c, $(COMMON_H)
test-not-readable-nor-writable-on-read-error.obj -
        : [-.test]test-not-readable-nor-writable-on-read-error.c, $(COMMON_H)
test-not-writable-after-shutdown.obj -
                : [-.test]test-not-writable-after-shutdown.c, $(COMMON_H)
test-osx-select.obj         : [-.test]test-osx-select.c, $(COMMON_H)
test-pass-always.obj        : [-.test]test-pass-always.c, $(COMMON_H)
test-ping-pong.obj          : [-.test]test-ping-pong.c, $(COMMON_H)
test-pipe-bind-error.obj    : [-.test]test-pipe-bind-error.c, $(COMMON_H)
test-pipe-close-stdout-read-stdin.obj -
                : [-.test]test-pipe-close-stdout-read-stdin.c, $(COMMON_H)
test-pipe-connect-error.obj : [-.test]test-pipe-connect-error.c, $(COMMON_H)
test-pipe-connect-multiple.obj -
                : [-.test]test-pipe-connect-multiple.c, $(COMMON_H)
test-pipe-connect-prepare.obj : [-.test]test-pipe-connect-prepare.c, $(COMMON_H)
test-pipe-getsockname.obj   : [-.test]test-pipe-getsockname.c, $(COMMON_H)
test-pipe-pending-instances.obj -
                : [-.test]test-pipe-pending-instances.c, $(COMMON_H)
test-pipe-sendmsg.obj       : [-.test]test-pipe-sendmsg.c, $(COMMON_H)
test-pipe-server-close.obj  : [-.test]test-pipe-server-close.c, $(COMMON_H)
test-pipe-set-fchmod.obj    : [-.test]test-pipe-set-fchmod.c, $(COMMON_H)
test-pipe-set-non-blocking.obj -
                : [-.test]test-pipe-set-non-blocking.c, $(COMMON_H)
test-platform-output.obj    : [-.test]test-platform-output.c, $(COMMON_H)
test-poll-close-doesnt-corrupt-stack.obj -
                : [-.test]test-poll-close-doesnt-corrupt-stack.c, $(COMMON_H)
test-poll-close.obj         : [-.test]test-poll-close.c, $(COMMON_H)
test-poll-closesocket.obj   : [-.test]test-poll-closesocket.c, $(COMMON_H)
test-poll-multiple-handles.obj -
                : [-.test]test-poll-multiple-handles.c, $(COMMON_H)
test-poll-oob.obj           : [-.test]test-poll-oob.c, $(COMMON_H)
test-poll.obj               : [-.test]test-poll.c, $(COMMON_H)
test-process-priority.obj   : [-.test]test-process-priority.c, $(COMMON_H)
test-process-title-threadsafe.obj -
                : [-.test]test-process-title-threadsafe.c, $(COMMON_H)
test-process-title.obj      : [-.test]test-process-title.c, $(COMMON_H)
test-queue-foreach-delete.obj : [-.test]test-queue-foreach-delete.c, $(COMMON_H)
test-random.obj             : [-.test]test-random.c, $(COMMON_H)
test-readable-on-eof.obj    : [-.test]test-readable-on-eof.c, $(COMMON_H)
test-ref.obj                : [-.test]test-ref.c, $(COMMON_H)
test-run-nowait.obj         : [-.test]test-run-nowait.c, $(COMMON_H)
test-run-once.obj           : [-.test]test-run-once.c, $(COMMON_H)
test-semaphore.obj          : [-.test]test-semaphore.c, $(COMMON_H)
test-shutdown-close.obj     : [-.test]test-shutdown-close.c, $(COMMON_H)
test-shutdown-eof.obj       : [-.test]test-shutdown-eof.c, $(COMMON_H)
test-shutdown-simultaneous.obj : [-.test]test-shutdown-simultaneous.c, $(COMMON_H)
test-shutdown-twice.obj     : [-.test]test-shutdown-twice.c, $(COMMON_H)
test-signal-multiple-loops.obj : [-.test]test-signal-multiple-loops.c, $(COMMON_H)
test-signal-pending-on-close.obj -
                : [-.test]test-signal-pending-on-close.c, $(COMMON_H)
test-signal.obj             : [-.test]test-signal.c, $(COMMON_H)
test-socket-buffer-size.obj : [-.test]test-socket-buffer-size.c, $(COMMON_H)
test-spawn.obj              : [-.test]test-spawn.c, $(COMMON_H)
test-stdio-over-pipes.obj   : [-.test]test-stdio-over-pipes.c, $(COMMON_H)
test-strscpy.obj            : [-.test]test-strscpy.c, $(COMMON_H)
test-strtok.obj             : [-.test]test-strtok.c, $(COMMON_H)
test-tcp-alloc-cb-fail.obj  : [-.test]test-tcp-alloc-cb-fail.c, $(COMMON_H)
test-tcp-bind-error.obj     : [-.test]test-tcp-bind-error.c, $(COMMON_H)
test-tcp-bind6-error.obj    : [-.test]test-tcp-bind6-error.c, $(COMMON_H)
test-tcp-close-accept.obj   : [-.test]test-tcp-close-accept.c, $(COMMON_H)
test-tcp-close-after-read-timeout.obj -
                : [-.test]test-tcp-close-after-read-timeout.c, $(COMMON_H)
test-tcp-close-while-connecting.obj -
                : [-.test]test-tcp-close-while-connecting.c, $(COMMON_H)
test-tcp-close.obj          : [-.test]test-tcp-close.c, $(COMMON_H)
test-tcp-close-reset.obj    : [-.test]test-tcp-close-reset.c, $(COMMON_H)
test-tcp-connect-error-after-write.obj -
                : [-.test]test-tcp-connect-error-after-write.c, $(COMMON_H)
test-tcp-connect-error.obj  : [-.test]test-tcp-connect-error.c, $(COMMON_H)
test-tcp-connect-timeout.obj : [-.test]test-tcp-connect-timeout.c, $(COMMON_H)
test-tcp-connect6-error.obj : [-.test]test-tcp-connect6-error.c, $(COMMON_H)
test-tcp-create-socket-early.obj -
                : [-.test]test-tcp-create-socket-early.c, $(COMMON_H)
test-tcp-flags.obj          : [-.test]test-tcp-flags.c, $(COMMON_H)
test-tcp-oob.obj            : [-.test]test-tcp-oob.c, $(COMMON_H)
test-tcp-open.obj           : [-.test]test-tcp-open.c, $(COMMON_H)
test-tcp-read-stop.obj      : [-.test]test-tcp-read-stop.c, $(COMMON_H)
test-tcp-read-stop-start.obj : [-.test]test-tcp-read-stop-start.c, $(COMMON_H)
test-tcp-rst.obj            : [-.test]test-tcp-rst.c, $(COMMON_H)
test-tcp-shutdown-after-write.obj -
                : [-.test]test-tcp-shutdown-after-write.c, $(COMMON_H)
test-tcp-try-write.obj      : [-.test]test-tcp-try-write.c, $(COMMON_H)
test-tcp-write-in-a-row.obj : [-.test]test-tcp-write-in-a-row.c, $(COMMON_H)
test-tcp-try-write-error.obj : [-.test]test-tcp-try-write-error.c, $(COMMON_H)
test-tcp-unexpected-read.obj : [-.test]test-tcp-unexpected-read.c, $(COMMON_H)
test-tcp-write-after-connect.obj -
                : [-.test]test-tcp-write-after-connect.c, $(COMMON_H)
test-tcp-write-fail.obj     : [-.test]test-tcp-write-fail.c, $(COMMON_H)
test-tcp-write-queue-order.obj -
                : [-.test]test-tcp-write-queue-order.c, $(COMMON_H)
test-tcp-write-to-half-open-connection.obj -
                : [-.test]test-tcp-write-to-half-open-connection.c, $(COMMON_H)
test-tcp-writealot.obj      : [-.test]test-tcp-writealot.c, $(COMMON_H)
test-test-macros.obj        : [-.test]test-test-macros.c, $(COMMON_H)
test-thread-affinity.obj    : [-.test]test-thread-affinity.c, $(COMMON_H)
test-thread-equal.obj       : [-.test]test-thread-equal.c, $(COMMON_H)
test-thread.obj             : [-.test]test-thread.c, $(COMMON_H)
test-threadpool.obj         : [-.test]test-threadpool.c, $(COMMON_H)
test-threadpool-cancel.obj  : [-.test]test-threadpool-cancel.c, $(COMMON_H)
test-timer-again.obj        : [-.test]test-timer-again.c, $(COMMON_H)
test-timer-from-check.obj   : [-.test]test-timer-from-check.c, $(COMMON_H)
test-timer.obj              : [-.test]test-timer.c, $(COMMON_H)
test-tmpdir.obj             : [-.test]test-tmpdir.c, $(COMMON_H)
test-tty-duplicate-key.obj  : [-.test]test-tty-duplicate-key.c, $(COMMON_H)
test-tty-escape-sequence-processing.obj -
                : [-.test]test-tty-escape-sequence-processing.c, $(COMMON_H)
test-tty.obj                : [-.test]test-tty.c, $(COMMON_H)
test-udp-alloc-cb-fail.obj  : [-.test]test-udp-alloc-cb-fail.c, $(COMMON_H)
test-udp-bind.obj           : [-.test]test-udp-bind.c, $(COMMON_H)
test-udp-connect.obj        : [-.test]test-udp-connect.c, $(COMMON_H)
test-udp-connect6.obj       : [-.test]test-udp-connect6.c, $(COMMON_H)
test-udp-create-socket-early.obj -
                : [-.test]test-udp-create-socket-early.c, $(COMMON_H)
test-udp-dgram-too-big.obj  : [-.test]test-udp-dgram-too-big.c, $(COMMON_H)
test-udp-ipv6.obj           : [-.test]test-udp-ipv6.c, $(COMMON_H)
test-udp-mmsg.obj           : [-.test]test-udp-mmsg.c, $(COMMON_H)
test-udp-multicast-interface.obj -
                : [-.test]test-udp-multicast-interface.c, $(COMMON_H)
test-udp-multicast-interface6.obj -
                : [-.test]test-udp-multicast-interface6.c, $(COMMON_H)
test-udp-multicast-join.obj : [-.test]test-udp-multicast-join.c, $(COMMON_H)
test-udp-multicast-join6.obj : [-.test]test-udp-multicast-join6.c, $(COMMON_H)
test-udp-multicast-ttl.obj  : [-.test]test-udp-multicast-ttl.c, $(COMMON_H)
test-udp-open.obj           : [-.test]test-udp-open.c, $(COMMON_H)
test-udp-options.obj        : [-.test]test-udp-options.c, $(COMMON_H)
test-udp-send-and-recv.obj  : [-.test]test-udp-send-and-recv.c, $(COMMON_H)
test-udp-send-hang-loop.obj : [-.test]test-udp-send-hang-loop.c, $(COMMON_H)
test-udp-send-immediate.obj : [-.test]test-udp-send-immediate.c, $(COMMON_H)
test-udp-sendmmsg-error.obj : [-.test]test-udp-sendmmsg-error.c, $(COMMON_H)
test-udp-send-unreachable.obj : [-.test]test-udp-send-unreachable.c, $(COMMON_H)
test-udp-try-send.obj       : [-.test]test-udp-try-send.c, $(COMMON_H)
test-udp-recv-in-a-row.obj  : [-.test]test-udp-recv-in-a-row.c, $(COMMON_H)
test-uname.obj              : [-.test]test-uname.c, $(COMMON_H)
test-walk-handles.obj       : [-.test]test-walk-handles.c, $(COMMON_H)
test-watcher-cross-stop.obj : [-.test]test-watcher-cross-stop.c, $(COMMON_H)


! Clean build artifacts
clean :
	delete *.exe;*, *.obj;*, *.olb;*
