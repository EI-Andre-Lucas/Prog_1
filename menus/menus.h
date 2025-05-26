#ifndef MENUS_H
#define MENUS_H

#include "../incidentes/incidentes.h"

// Funções de interface
void limparEcra();
void clicarEnter();
void menuLoginRegistro();
void menuPrincipal(USERS* user);
void menuAdministrador(USERS* user);
void menuTecnico(USERS* user);

// Funções de gestão de incidentes
void menuIncidentesAdmin(Incidente** lista_incidentes);
void menuIncidentesTecnico(Incidente** lista_incidentes, const char* tecnico);

// Funções auxiliares de incidentes
void listarIncidentes(Incidente* lista);
void listarIncidentesPorEstado(Incidente* lista, EstadoIncidente estado);
void listarIncidentesPorSeveridade(Incidente* lista, Severidade severidade);
void listarIncidentesPorTipo(Incidente* lista, TipoIncidente tipo);
void listarIncidentesPorTecnico(Incidente* lista, const char* tecnico);
void listarIncidentesPorPeriodo(Incidente* lista, time_t inicio, time_t fim);
void mostrarDetalhesIncidente(Incidente* incidente);
void mostrarHistoricoIncidente(Incidente* incidente);
void mostrarFerramentasUtilizadas(Incidente* incidente);
void calcularTempoMedioResolucao(Incidente* lista, const char* tecnico);

#endif 