#ifndef MENUS_H
#define MENUS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../users/users.h"
#include "../incidentes/incidentes.h"
#include "../utils/utils.h"

// Funções de interface
void menuLoginRegisto();
void menuPrincipal(USERS* user);
void menuAdministrador(USERS* user);
void menuTecnico(USERS* user);

// Funções de gestão de incidentes
void menuIncidentesAdmin(Incidente** lista_incidentes);
void menuIncidentesTecnico(Incidente** lista_incidentes, const char* tecnico);

// Funções auxiliares de incidentes
void fornecerIncidentes(Incidente* lista);
void fornecerIncidentesPorEstado(Incidente* lista, EstadoIncidente estado);
void fornecerIncidentesPorSeveridade(Incidente* lista, Severidade severidade);
void fornecerIncidentesPorTipo(Incidente* lista, TipoIncidente tipo);
void fornecerIncidentesPorTecnico(Incidente* lista, const char* tecnico);
void fornecerIncidentesPorPeriodo(Incidente* lista, time_t inicio, time_t fim);
void mostrarDetalhesIncidente(Incidente* incidente);
void mostrarHistoricoIncidente(Incidente* incidente);
void mostrarFerramentasUtilizadas(Incidente* incidente);
void calcularTempoMedioResolucao(Incidente* lista, const char* tecnico);

void menuGestaoUsers();
void menuLogs();

#endif 