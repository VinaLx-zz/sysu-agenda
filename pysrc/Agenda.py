from AgendaUI import AgendaUI


if __name__ == '__main__':
    ui = AgendaUI()
    try:
        ui.operationLoop()
    except KeyboardInterrupt:
        ui.quitAgenda()
