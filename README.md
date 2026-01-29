# jobd – Job Runner / Scheduler

## Objetivo

Daemon en C para ejecutar jobs (comandos) con control de reintentos, timeouts y logging estructurado. Útil para uso diario (scripts, backups, tareas batch).

## Alcance MVP

* Ejecutar jobs definidos en un archivo (`jobs.conf`)
* Manejo de procesos (`fork/exec`)
* Captura de stdout/stderr
* Retries con backoff simple
* Timeout por job
* Exit codes
* Logs en JSON
* Señales: SIGTERM / SIGINT (shutdown limpio)

## Fuera de alcance (v1)

* UI
* Persistencia avanzada
* HTTP API
* Programación tipo cron (v1.1)

---

## Estructura del proyecto

```
jobd/
 ├─ src/
 │   ├─ main.c
 │   ├─ config.c / config.h
 │   ├─ runner.c / runner.h
 │   ├─ logger.c / logger.h
 │   ├─ process.c / process.h
 │   └─ signal.c / signal.h
 ├─ include/
 ├─ jobs.conf
 └─ Makefile
```

---

## Formato de configuración (simple)

Archivo `jobs.conf` (ini-like):

```
[job]
name = backup_db
cmd = /usr/bin/pg_dump mydb
retries = 3
timeout = 30

[job]
name = cleanup
cmd = /usr/bin/find /tmp -type f -mtime +7 -delete
retries = 1
timeout = 10
```

---

## Flujo de ejecución

1. `jobd` inicia
2. Lee `jobs.conf`
3. Por cada job:

   * fork()
   * execve()
   * captura stdout/stderr via pipes
   * espera con timeout
   * si falla → retry
4. Loggea resultado
5. Maneja señales
6. Shutdown limpio

---

## Logging (JSON)

Ejemplo:

```
{"ts":"2026-01-29T18:30:00Z","level":"info","job":"backup_db","status":"started"}
{"ts":"2026-01-29T18:30:10Z","level":"error","job":"backup_db","exit":1,"retry":1}
```

---

## Decisiones técnicas

* `select()` para timeout
* pipes para stdout/stderr
* sin threads en MVP
* POSIX puro

---

## Próximos pasos

* v1.1: cron-like scheduler
* v1.2: HTTP status endpoint
* v1.3: métricas

