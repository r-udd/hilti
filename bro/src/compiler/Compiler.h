///
/// Bro script compiler providing the main entry point to compiling a
/// complete Bro configuration into HILTI.
///

#ifndef BRO_PLUGIN_HILTI_COMPILER_COMPILER_H
#define BRO_PLUGIN_HILTI_COMPILER_COMPILER_H

#include <list>
#include <vector>
#include <set>
#include <memory>

class Func;
class ID;

namespace hilti {
	class CompilerContext;
	class Module;
	class Type;

	namespace builder {
		class BlockBuilder;
		class ModuleBuilder;
	}
}

namespace bro {
namespace hilti {

namespace compiler {

class CollectorCallback;
class ModuleBuilder;

class Compiler {
public:
	typedef std::list<std::shared_ptr<::hilti::Module>> module_list;

	/**
	 * Constructor.
	 *
	 * ctx: The HILTI context to use for compiling script code. */
	Compiler(std::shared_ptr<::hilti::CompilerContext> ctx);

	/**
	 * Destructor.
	 */
	~Compiler();

	/**
	 * Compiles all of Bro's script code.
	 *
	 * @return A list of compiled modules.
	 */
	module_list CompileAll();

	/**
	 * Loads one external *.hlt file.
	 *
	 * @param path The full path to load the file from.
	 *
	 * @return True if successfull.
	 */
	bool LoadExternalHiltiCode(const std::string& path);

	/**
	 * Returns all of a Bro namespace's function that need to be
	 * compiled.
	 */
	std::list<const Func *> Functions(const string& ns);

	/**
	 * Returns all of a Bro namespace's global IDs that need to be
	 * compiled.
	 */
	std::list<const ::ID *> Globals(const string& ns);

	/**
	 * Returns the module builder currently in use.
	 */
	::hilti::builder::ModuleBuilder* ModuleBuilder();

	/**
	 * Returns the current block builder. This is a short-cut to calling
	 * the current corresponding method of the current module builder.
	 *
	 * @return The block builder.
	 */
	std::shared_ptr<::hilti::builder::BlockBuilder> Builder() const;

	/**
	 * Pushes another HILTI module builder on top of the current stack.
	 * This one will now be consulted by Builder().
	 */
	void pushModuleBuilder(::hilti::builder::ModuleBuilder* mbuilder);

	/**
	 * Removes the top-most HILTI module builder from the stack. This
	 * must not be called more often than pushModuleBuilder().
	 */
	void popModuleBuilder();

	/**
	 * Returns the module builder for the glue code module.
	 *
	 * TODO: We should move the glue builder over to the manager.
	 */
	::hilti::builder::ModuleBuilder* GlueModuleBuilder() const;

	/**
	 * XXXX
	 */
	class ModuleBuilder* moduleBuilderForNamespace(const std::string& ns);

	/**
	 * XXXX
	 */
	std::shared_ptr<::hilti::Module> FinalizeGlueBuilder();

	/**
	 * Returns the internal HILTI-level symbol for a Bro Function.
	 *
	 * @param func The function.
	 *
	 * @param module If non-null, a module to which the returned symbol
	 * should be relative. If the function's ID has the same namespace as
	 * the module, it will be skipped; otherwise included.
	 */
	std::string HiltiSymbol(const ::Func* func, shared_ptr<::hilti::Module> module, bool include_module = false);

	/**
	 * Returns the internal HILTI-level symbol for the stub of a Bro Function.
	 *
	 * @param func The function.
	 *
	 * @param module If non-null, a module to which the returned symbol
	 * should be relative. If the function's ID has the same namespace as
	 * the module, it will be skipped; otherwise included.
	 *
	 * @param include_module If true, the returned name will include the
	 * module name and hence reoresent the symbol as visibile at the LLVM
	 * level after linking.
	 */
	std::string HiltiStubSymbol(const ::Func* func, shared_ptr<::hilti::Module> module, bool include_module);

	/**
	 * Returns the internal HILTI-level symbol for a Bro ID.
	 *
	 * @param id The ID.
	 *
	 * @param module: If non-null, a module to which the returned symbol
	 * should be relative. If the function's ID has the same namespace as
	 * the module, it will be skipped; otherwise included.
	 */
	std::string HiltiSymbol(const ::ID* id, shared_ptr<::hilti::Module> module);

	/**
	 * Returns the internal HILTI-level symbol for a Bro type. This will
	 * always be a globally valid ID.
	 *
	 * @param t The type.
	 */
	std::string HiltiSymbol(const ::BroType* );

	/**
	 * Returns the internal HILTI-level symbol for a Bro ID.
	 *
	 * @param id The ID.
	 *
	 * @param global True if this is a global ID that need potentially needs
	 * to be qualified with a namespace.
	 *
	 * @param module: If non-empty, a module name to which the returned
	 * symbol should be relative. If the function's ID has the same
	 * namespace as the module, it will be skipped; otherwise included.
	 *
	 * @param include_module If true, the returned name will include the
	 * module name and hence reoresent the symbol as visibile at the LLVM
	 * level after linking.
	 */
	std::string HiltiSymbol(const std::string& id, bool global, const std::string& module, bool include_module = false);

	/**
	 * Renders a \a BroObj via its \c Describe() method and turns the
	 * result into a valid HILTI identifier string.
	 *
	 * @param obj The object to describe.
	 *
	 * @return A string with a valid HILTI identifier.
	 */
	std::string HiltiODescSymbol(const ::BroObj* obj);

	/**
	 * Registers a function as having been compiled.
	 */
	void RegisterCompiledFunction(const ::Func* func);

	typedef std::map<std::string, const ::Func*> function_symbol_map;

	/**
	 * Returns a map mapping the HILTI symbols of all compiled scripts
	 * functions to their corresponding Bro functions.
	 */
	const function_symbol_map& HiltiFunctionSymbolMap() const;

	/**
	 * Checks if a built-in Bro function is available at HILTI-level.
	 *
	 * @param The fully-qualified Bro-level name of the function.
	 *
	 * @param If given, the HILTI-level name of the function will be
	 * stored in here.
	 *
	 * @param True if the BiF is available at the HILTI-level.
	 */
	bool HaveHiltiBif(const std::string& name, std::string* hilti_name = 0);

	/**
	 * Attempts to statically determine the Bro function referenced by a
	 * Bro expression. This may or may not work.
	 *
	 * @param func The expression referencing a function
	 *
	 * @return A pair \a (success, function). If \a success is true, we
	 * could infer which Bro function the expression referes to; in that
	 * case, the function is usually it in the 2nd pair element if
	 * there's a local implementation (and null instead if not). If \a
	 * success is false, we couldn't statically determine behind the
	 * expression; in that case, the 2nd pair element is undefined.
	 */
	std::pair<bool, ::Func*> BroExprToFunc(const ::Expr* func);

	/**
	 * XXX
	 */
	void CacheCustomBroType(const BroType* btype, shared_ptr<::hilti::Type> htype, const std::string& bro_id_name);

	/**
	 * XXX
	 */
	std::pair<shared_ptr<::hilti::Type>, std::string> LookupCachedCustomBroType(const BroType* btype);

	/**
	 * XXX
	 */
	bool HaveCustomHandler(const ::Func* ev);

private:
	std::string normalizeSymbol(const std::string sym, const std::string prefix, const std::string postfix, const std::string& module, bool global, bool include_module = false);
	bool CompileScripts();
	Compiler::module_list CompileExternalModules();
	std::list<std::string> GetNamespaces() const;

	std::shared_ptr<::hilti::CompilerContext> hilti_context;

	typedef std::list<::hilti::builder::ModuleBuilder*> module_builder_list;
	module_builder_list mbuilders;

	shared_ptr<::hilti::builder::ModuleBuilder> glue_module_builder;

	std::shared_ptr<CollectorCallback> collector_callback;

	std::map<const BroType*, std::pair<shared_ptr<::hilti::Type>, std::string>> cached_custom_types;
	function_symbol_map hilti_function_symbol_map;

	std::list<std::string> external_modules; // Custom modules loaded from external files.

	typedef std::map<string, std::shared_ptr<class ModuleBuilder>> mbuilder_map;
	mbuilder_map mbuilders_by_ns;

	std::vector<bool> custom_event_handlers;
};

}
}
}

#endif
