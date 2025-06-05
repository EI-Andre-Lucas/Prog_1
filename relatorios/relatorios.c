void gerarRelatorio(ELEM* lista_incidentes, USERS* lista_utilizadores) {
    if (!lista_incidentes) {
        printf("Não há incidentes para gerar relatório!\n");
        return;
    }

    FILE* file = fopen("relatorio.txt", "w");
    if (!file) {
        printf("Erro ao criar arquivo de relatório!\n");
        return;
    }

    // Estatísticas gerais
    int total_incidentes = 0;
    int incidentes_abertos = 0;
    int incidentes_em_atendimento = 0;
    int incidentes_resolvidos = 0;
    int total_respostas = 0;
    int total_acoes = 0;
    int total_utilizadores = 0;
    int total_tecnicos = 0;
    int total_admins = 0;

    // Contar utilizadores
    USERS* current_user = lista_utilizadores;
    while (current_user) {
        total_utilizadores++;
        if (current_user->tipoUser == TECNICO) total_tecnicos++;
        if (current_user->tipoUser == ADMINISTRADOR) total_admins++;
        current_user = current_user->proximo;
    }

    // Contar incidentes e suas estatísticas
    ELEM* current = lista_incidentes;
    while (current) {
        total_incidentes++;
        
        if (strcmp(current->incidente.estado, "Aberto") == 0) incidentes_abertos++;
        else if (strcmp(current->incidente.estado, "Em Atendimento") == 0) incidentes_em_atendimento++;
        else if (strcmp(current->incidente.estado, "Resolvido") == 0) incidentes_resolvidos++;

        // Contar respostas e ações
        total_respostas += current->incidente.num_respostas;
        total_acoes += current->incidente.num_acoes;

        current = current->proximo;
    }

    // Cabeçalho do relatório
    fprintf(file, "=== RELATÓRIO DO SISTEMA ===\n\n");
    fprintf(file, "Data de geração: %s\n\n", obterDataAtual());

    // Estatísticas de utilizadores
    fprintf(file, "=== ESTATÍSTICAS DE UTILIZADORES ===\n");
    fprintf(file, "Total de utilizadores: %d\n", total_utilizadores);
    fprintf(file, "- Administradores: %d\n", total_admins);
    fprintf(file, "- Técnicos: %d\n", total_tecnicos);
    fprintf(file, "- Utilizadores normais: %d\n\n", total_utilizadores - total_admins - total_tecnicos);

    // Estatísticas de incidentes
    fprintf(file, "=== ESTATÍSTICAS DE INCIDENTES ===\n");
    fprintf(file, "Total de incidentes: %d\n", total_incidentes);
    fprintf(file, "- Em aberto: %d\n", incidentes_abertos);
    fprintf(file, "- Em atendimento: %d\n", incidentes_em_atendimento);
    fprintf(file, "- Resolvidos: %d\n\n", incidentes_resolvidos);

    // Estatísticas de interações
    fprintf(file, "=== ESTATÍSTICAS DE INTERAÇÕES ===\n");
    fprintf(file, "Total de respostas: %d\n", total_respostas);
    fprintf(file, "Total de ações no histórico: %d\n", total_acoes);
    fprintf(file, "Média de respostas por incidente: %.2f\n", 
            total_incidentes > 0 ? (float)total_respostas/total_incidentes : 0);
    fprintf(file, "Média de ações por incidente: %.2f\n\n", 
            total_incidentes > 0 ? (float)total_acoes/total_incidentes : 0);

    // Lista detalhada de incidentes
    fprintf(file, "=== LISTA DETALHADA DE INCIDENTES ===\n\n");
    current = lista_incidentes;
    while (current) {
        fprintf(file, "ID: %d\n", current->incidente.id);
        fprintf(file, "Título: %s\n", current->incidente.titulo);
        fprintf(file, "Estado: %s\n", current->incidente.estado);
        fprintf(file, "Prioridade: %s\n", current->incidente.prioridade);
        fprintf(file, "Categoria: %s\n", current->incidente.categoria);
        fprintf(file, "Criado por: %s\n", current->incidente.username_criador);
        fprintf(file, "Técnico responsável: %s\n", current->incidente.tecnico_responsavel);
        fprintf(file, "Data de criação: %s\n", current->incidente.data_criacao);
        fprintf(file, "Última atualização: %s\n", current->incidente.data_atualizacao);
        fprintf(file, "Número de respostas: %d\n", current->incidente.num_respostas);
        fprintf(file, "Número de ações: %d\n", current->incidente.num_acoes);
        fprintf(file, "Número de ferramentas: %d\n", current->incidente.num_ferramentas);
        fprintf(file, "\n");

        current = current->proximo;
    }

    fclose(file);
    printf("Relatório gerado com sucesso em 'relatorio.txt'!\n");
} 