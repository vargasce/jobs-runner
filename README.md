# jobd – Job Runner / Scheduler

## Objetivo

Daemon en C para ejecutar jobs (comandos) con control de reintentos, timeouts y logging estructurado. Útil para uso diario (scripts, backups, tareas batch).

## Alcance MVP

- Ejecutar jobs definidos en un archivo (`jobs.conf`)
- Manejo de procesos (`fork/exec`)
- Captura de stdout/stderr
- Retries con backoff simple
- Timeout por job
- Exit codes
- Logs en JSON
- Señales: SIGTERM / SIGINT (shutdown limpio)

## Fuera de alcance (v1)

- UI
- Persistencia avanzada
- HTTP API
- Programación tipo cron (v1.1)

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

Archivo `jobs.json` (ini-like):

```
{
  "jobs": [
    {
      "name": "Proccess 1",
      "command": "random_sleep.sh",
      "timeout": "",
      "path": "./"
    },
    {
      "name": "Proccess 2",
      "command": "random_sleep.sh",
      "timeout": "",
      "path": "./"
    },
  ]
}
```

---

## Flujo de ejecución

1. `jobd` inicia
2. Lee `jobs.json`
3. Por cada job:
   - fork()
   - execve()
   - captura stdout/stderr via pipes
   - espera con timeout
   - si falla → retry

4. Loggea resultado
5. Maneja señales
6. Shutdown limpio

---

## Decisiones técnicas

- `select()` para timeout
- pipes para stdout/stderr
- sin threads en MVP
- POSIX puro

---

## Próximos pasos

- v1.1: cron-like scheduler
- v1.2: HTTP status endpoint
- v1.3: métricas
