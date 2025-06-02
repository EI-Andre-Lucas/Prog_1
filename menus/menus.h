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
void menuPrincipal();
void menuAdministrador();
void menuTecnico();

// Funções de gestão de incidentes
void menuIncidentesAdmin();
void menuIncidentesTecnico(ELEM** lista_incidentes, const char* tecnico);

// Funções auxiliares de incidentes
void fornecerIncidentes(ELEM* lista);
void fornecerIncidentesPorEstado(ELEM* lista, EstadoIncidente estado);
void fornecerIncidentesPorSeveridade(ELEM* lista, Severidade severidade);
void fornecerIncidentesPorTipo(ELEM* lista, TipoIncidente tipo);
void fornecerIncidentesPorTecnico(ELEM* lista, const char* tecnico);
void fornecerIncidentesPorPeriodo(ELEM* lista, time_t inicio, time_t fim);
void mostrarDetalhesIncidente(ELEM* elem);
void mostrarHistoricoIncidente(ELEM* elem);
void mostrarFerramentasUtilizadas(ELEM* elem);
void calcularTempoMedioResolucao(ELEM* lista, const char* tecnico);

void menuGestaoUsers();
void menuLogs();

#endif 