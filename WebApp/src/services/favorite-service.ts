import { LocalStorageService } from "./local-storage-service";

/**
 * FavoriteServiceController
 */
export class FavoriteServiceController {

  /**
   * file
   */
  private file: string = '/assets/data/favorites.json';

  /**
   * localStorageService
   */
  private localStorageService: any = LocalStorageService;

  /**
   * storageKey
   */
  private storageKey: string = 'favorites';

  /**
   * add
   * @param item
   */
  add = async (item: Favorite): Promise<void> => {
    let items = await this.localStorageService.get(this.storageKey) || [];
    items.push(item);
    await this.localStorageService.set(this.storageKey, items);
  };

  /**
   * remove
   * @param item
   */
  remove = async (item: Favorite): Promise<void> => {
    let items = await this.localStorageService.get(this.storageKey);
    for (let i = 0; i < items.length; i++) {
      // Hm, why do i have to check the strings?
      // A comparision of both objects fails.
      if (JSON.stringify(items[i]) === JSON.stringify(item)) {
        items.splice(i, 1);
      }
    }
    await this.localStorageService.set(this.storageKey, items);
  };

  /**
   * get
   */
  get = async (): Promise<Favorite[]> => {
    let favorites = await this.localStorageService.get(this.storageKey) || [];
    fetch(this.file)
      .then(response => response.json())
      .then(data => {
        data.map(item => {
          if (!this.containsObject(item, favorites)) {
            this.add(item);
          }
        });
      });
    return await this.localStorageService.get(this.storageKey) || [];
  };

  /**
   * containsObject
   * @param obj
   * @param list
   */
  containsObject = (obj, list) => {
    for (let i = 0; i < list.length; i++) {
      if (JSON.stringify(list[i]) === JSON.stringify(obj)) {
        return true;
      }
    }
    return false;
  }
}

export const FavoriteService = new FavoriteServiceController();
