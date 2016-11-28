SUBDIRS = game-engine game-ai

all: $(SUBDIRS)

clean: $(SUBDIRS)

.PHONY: $(SUBDIRS)
$(SUBDIRS):
	@echo "===>" $@
		$(MAKE) -C $@ $(MAKECMDGOALS);
	@echo "<===" $@
